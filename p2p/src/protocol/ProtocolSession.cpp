#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "ProtocolSession.h"
#include "MessageFramer.h"
#include "../core/Logger.h"
#include "../storage/ChunkManager.h"
#include "../storage/FileManager.h"

#include<cstring>

ProtocolSession::ProtocolSession(TcpConnection&& conn,const std::string &selfId,uint32_t max_allowed_size,uint32_t chunksz):connection(std::move(conn)),selfPeerId(selfId),max_allowed_size(max_allowed_size),chunkSize(chunksz){this->crypto=nullptr;}

std::string ProtocolSession::getRemotePeerId()const{
    return remotePeerId;
}

bool ProtocolSession::sendEncryptedMessage(Message &msg){
    msg.payload=crypto->encrypt(msg.payload);
    msg.header.length=msg.payload.size();

    auto encoded=MessageFramer::encode(msg,max_allowed_size);
    connection.sendBytes(encoded);
    return true;
}

bool ProtocolSession::recvEncryptedMessage(Message &out){
    vector<uint8_t> buffer;
    while(true){
        int r=connection.recvBytes(buffer);
        if(r<=0)return false;
        
        if(MessageFramer::decode(buffer,out,max_allowed_size)){
            out.payload=crypto->decrypt(out.payload);
            out.header.length=out.payload.size();
            return true;
        }
    }
}

bool ProtocolSession::performKeyExchange(){
    Message m;
    m.header.type=MessageType::MSG_KEY_EXCHG;
    m.payload=keyEx.getPublicKey();
    m.header.length=m.payload.size();

    connection.sendBytes(MessageFramer::encode(m,max_allowed_size));
    
    Message resp;
    vector<uint8_t> buf;
    while(true){
        connection.recvBytes(buf);
        if(MessageFramer::decode(buf,resp,max_allowed_size))break;
    }
    auto secret=keyEx.deriveSharedKey(resp.payload);
    crypto=new CryptoEngine(secret);
    return true;
}

bool ProtocolSession::sendFile(const std::string&filePath,const string&metaPath){
    ChunkManager cm(filePath,chunkSize);
    if(!cm.generateManifest(metaPath)){
        Logger::instance().error("Failed to generate manifest");
        return false;
    }

    std::ifstream metaFile(metaPath,std::ios::binary | std::ios::ate);
    if(!metaFile){
        Logger::instance().error("Failed to open manifest file");
        return false;
    }

    std::streamsize metaSize=metaFile.tellg();
    metaFile.seekg(0,std::ios::beg);
    std::vector<uint8_t> manifestData(metaSize);
    metaFile.read((char*)manifestData.data(),metaSize);

    size_t fileSize=FileManager::getFileSize(filePath);
    uint32_t totalChunks=(fileSize+chunkSize-1)/chunkSize;
    uint32_t mSize=(uint32_t)manifestData.size();

    std::vector<uint8_t>payload;

    payload.insert(payload.end(),(uint8_t*)&fileSize,(uint8_t*)&fileSize+sizeof(fileSize));
    payload.insert(payload.end(),(uint8_t*)&chunkSize,(uint8_t*)&chunkSize+sizeof(chunkSize));
    payload.insert(payload.end(),(uint8_t*)&totalChunks,(uint8_t*)&totalChunks+sizeof(totalChunks));
    payload.insert(payload.end(),(uint8_t*)&mSize,(uint8_t*)&mSize+sizeof(mSize));
    payload.insert(payload.end(),manifestData.begin(),manifestData.end());
    payload.insert(payload.end(),filePath.begin(),filePath.end());

    Message offer;
    offer.header.type=MessageType::MSG_FILE_OFFER;
    offer.payload=payload;

    if(!sendEncryptedMessage(offer)){
        Logger::instance().error("Failed to send MSG_FILE_OFFER");
        return false;
    }

    //Wait for file accept
    Message resp;
    if(!recvEncryptedMessage(resp)){
        Logger::instance().error("Failed to receive MSG_FILE_ACCEPT");
        return false;
    }

    //Check file accept response
    if(resp.header.type!=MessageType::MSG_FILE_ACCEPT){
        Logger::instance().error("Expected MSG_FILE_ACCEPT");
        return false;
    }

    //send file in chunks
    for(uint32_t i=0;i<totalChunks;i++){
        size_t offset=i*chunkSize;
        size_t size=std::min(chunkSize,(uint32_t)(fileSize-offset));
        std::vector<uint8_t> chunk;
        FileManager::readChunk(filePath,offset,size,chunk);
        Message chunkMsg;
        chunkMsg.header.type=MessageType::MSG_CHUNK_DATA;
        chunkMsg.payload=chunk;
        if(!sendEncryptedMessage(chunkMsg)){
            Logger::instance().error("Failed to send MSG_FILE_CHUNK at chunk "+std::to_string(i));
            return false;
        }else{
            Logger::instance().info("Sent chunk "+std::to_string(i));
        }
        Message ack;
        std::vector<uint8_t> abuf;
        if(!recvEncryptedMessage(ack)){
            Logger::instance().error("Failed to receive MSG_FILE_CHUNK_ACK at chunk "+std::to_string(i));
            return false;
        }else{
            Logger::instance().info("Received chunk ack "+std::to_string(i));
        }
        if(ack.header.type!=MessageType::MSG_CHUNK_ACK){
            Logger::instance().error("Expected MSG_FILE_CHUNK_ACK at chunk "+std::to_string(i));
            return false;
        }
    }
    return true;
}

bool ProtocolSession::recvFile(const std::string&outputPath){
    Message offer;
    if(!recvEncryptedMessage(offer)){
        Logger::instance().error("Failed to receive MSG_FILE_OFFER");
        return false;
    }
    
    if(offer.header.type!=MessageType::MSG_FILE_OFFER){
        Logger::instance().error("Expected MSG_FILE_OFFER");
        return false;
    }

    uint64_t fileSize;
    uint32_t cSize,totalChunks,mSize;
    uint8_t* ptr=offer.payload.data();
    memcpy(&fileSize,ptr,sizeof(fileSize));
    ptr+=sizeof(fileSize);
    memcpy(&cSize,ptr,sizeof(cSize));
    ptr+=sizeof(cSize);
    memcpy(&totalChunks,ptr,sizeof(totalChunks));
    ptr+=sizeof(totalChunks);
    memcpy(&mSize,ptr,sizeof(mSize));
    ptr+=sizeof(mSize);

    //Extract manifest bytes and parse into hashes
    std::vector<uint8_t> manifestData(ptr,ptr+mSize);
    ChunkManager cmHelper("",0);
    std::vector<string> expectedHashes=cmHelper.parseManifestData(manifestData);

    //accept
    Message accept;
    accept.header.type=MessageType::MSG_FILE_ACCEPT;
    if(!sendEncryptedMessage(accept)){
        Logger::instance().error("Failed to send MSG_FILE_ACCEPT");
        return false;
    }

    std::ofstream out(outputPath,std::ios::binary);
    for(uint32_t i=0;i<totalChunks;i++){
         Message chunk;
         std::vector<uint8_t> cbuf;
         if(!recvEncryptedMessage(chunk)){
             Logger::instance().error("Failed to receive MSG_FILE_CHUNK");
             return false;
         }else{
             Logger::instance().info("Received chunk "+std::to_string(i));
         }

         //hash verify
         string actualHash=cmHelper.sha256(chunk.payload);
         if(actualHash!=expectedHashes[i]){
             Logger::instance().error("Hash mismatch for chunk "+std::to_string(i));
             return false;
         }else{
             Logger::instance().info("Verified chunk "+std::to_string(i));
         }
         out.write((char*)chunk.payload.data(),chunk.payload.size());

         Message ack;
         ack.header.type=MessageType::MSG_CHUNK_ACK;
         if(!sendEncryptedMessage(ack)){
             Logger::instance().error("Failed to send MSG_FILE_CHUNK_ACK");
             return false;
         }else{
             Logger::instance().info("Sent chunk ack "+std::to_string(i));
         }
    }
    out.close();
    return true;
}

//                    ---CLIENT---
bool ProtocolSession::performClientHandshake(){
    if(!sendHello()){
        Logger::instance().error("Failed to send MSG_HELLO");
        return false;
    }

    Message ack;
    if(!recvHelloAck(ack)) return false;

    if(ack.header.type!=MessageType::MSG_HELLO_ACK){
        Logger::instance().error("Expected MSG_HELLO_ACK");
        return false;
    }
    Logger::instance().info("Handshake successful with "+remotePeerId);
    return true;
}

//                    ---SERVER---
bool ProtocolSession::performServerHandshake(){
    Message hello;
    if(!recvHello(hello)) return false;

    if(hello.header.type!=MessageType::MSG_HELLO){
        Logger::instance().error("Expected MSG_HELLO");
        return false;
    }
    if(!sendHelloAck()){
        Logger::instance().error("Failed to send MSG_HELLO_ACK");
        return false;
    }
    Logger::instance().info("Handshake successful with "+remotePeerId);
    return true;
}

//                    ---INTERNAL---
bool ProtocolSession::sendHello(){
    uint16_t version=PROTOCOL_VERSION;
    uint32_t caps=0x01 | 0x02; //File + Resume

    std::vector<uint8_t> payload;
    payload.insert(payload.end(),reinterpret_cast<uint8_t*>(&version),reinterpret_cast<uint8_t*>(&version)+sizeof(version));
    payload.insert(payload.end(),reinterpret_cast<uint8_t*>(&caps),reinterpret_cast<uint8_t*>(&caps)+sizeof(caps));
    payload.insert(payload.end(),selfPeerId.begin(),selfPeerId.end());
    
    Message msg;
    msg.header.type=MessageType::MSG_HELLO;
    msg.header.length=payload.size();
    msg.payload=payload;
    
   auto encoded=MessageFramer::encode(msg,max_allowed_size);
   connection.sendBytes(encoded);

   return true;
}

bool ProtocolSession::sendHelloAck(){
    uint16_t version=PROTOCOL_VERSION;
    uint32_t caps=0x01 | 0x02; //File + Resume

    std::vector<uint8_t> payload;
    payload.insert(payload.end(),reinterpret_cast<uint8_t*>(&version),reinterpret_cast<uint8_t*>(&version)+sizeof(version));
    payload.insert(payload.end(),reinterpret_cast<uint8_t*>(&caps),reinterpret_cast<uint8_t*>(&caps)+sizeof(caps));
    payload.insert(payload.end(),selfPeerId.begin(),selfPeerId.end());

    Message msg;
    msg.header.type=MessageType::MSG_HELLO_ACK;
    msg.header.length=payload.size();
    msg.payload=payload;

    auto encoded=MessageFramer::encode(msg,max_allowed_size);
    connection.sendBytes(encoded);
    return true;
}

bool ProtocolSession::recvHello(Message &out){
    std::vector<uint8_t> buffer;
    while(true){
        int r=connection.recvBytes(buffer);
        if(r<=0)return false;

        if(MessageFramer::decode(buffer,out,max_allowed_size))break;
    }
    if(out.header.type==MessageType::MSG_HELLO){
        uint16_t version;
        uint32_t caps;

        std::memcpy(&version,out.payload.data(),sizeof(version));
        std::memcpy(&caps,out.payload.data()+sizeof(version),sizeof(caps));

        if(version!=PROTOCOL_VERSION){
            Logger::instance().error("Protocol Version mismatch");
            return false;
        }

        remotePeerId.assign(reinterpret_cast<char*>(out.payload.data()+sizeof(version)+sizeof(caps)),out.payload.size()-sizeof(version)-sizeof(caps));
        return true;
    }else return false;
}

bool ProtocolSession::recvHelloAck(Message &out){
     std::vector<uint8_t> buffer;
    while(true){
        int r=connection.recvBytes(buffer);
        if(r<=0)return false;

        if(MessageFramer::decode(buffer,out,max_allowed_size))break;
    }
    if(out.header.type==MessageType::MSG_HELLO_ACK){
        uint16_t version;
        uint32_t caps;

        std::memcpy(&version,out.payload.data(),sizeof(version));
        std::memcpy(&caps,out.payload.data()+sizeof(version),sizeof(caps));

        if(version!=PROTOCOL_VERSION){
            Logger::instance().error("Protocol Version mismatch");
            return false;
        }

        remotePeerId.assign(reinterpret_cast<char*>(out.payload.data()+sizeof(version)+sizeof(caps)),out.payload.size()-sizeof(version)-sizeof(caps));
        return true;
    }else return false;
}