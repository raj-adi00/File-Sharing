#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "ProtocolSession.h"
#include "MessageFramer.h"
#include "../core/Logger.h"

#include<cstring>

ProtocolSession::ProtocolSession(TcpConnection&& conn,const std::string &selfId,uint32_t max_allowed_size):connection(std::move(conn)),selfPeerId(selfId),max_allowed_size(max_allowed_size){}

std::string ProtocolSession::getRemotePeerId()const{
    return remotePeerId;
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