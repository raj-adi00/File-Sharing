#include "MessageFramer.h"
#include <cstring>

using namespace std;

vector<uint8_t> MessageFramer::encode(const Message & msg,uint32_t max_allowed_size){
    
    if(msg.payload.size()>max_allowed_size){
        return {};
    }
    vector<uint8_t> data;
    uint16_t type=static_cast<uint16_t>(msg.header.type);
    uint32_t len=msg.header.length;

    data.resize(sizeof(type)+sizeof(len)+msg.payload.size());

    size_t offset=0;
    memcpy(data.data()+offset,&type,sizeof(type));
    offset+=sizeof(type);

    memcpy(data.data()+offset,&len,sizeof(len));
    offset+=sizeof(len);

    if(!msg.payload.empty()){
        memcpy(data.data()+offset,msg.payload.data(),msg.payload.size());
    }
    return data;
}
/*
 buffer: stream buffer (TCP)
 out   : decoded message
 returns true if a full message was decoded
*/
bool MessageFramer::decode(vector<uint8_t>& buffer,Message & out,uint32_t max_allowed_size){
    if(buffer.size()<sizeof(uint16_t)+sizeof(uint32_t))return false;

    uint16_t type_raw;
    uint32_t len_raw;

    memcpy(&type_raw,buffer.data(),sizeof(type_raw));
    memcpy(&len_raw,buffer.data()+sizeof(type_raw),sizeof(len_raw));

    uint32_t len=len_raw;

    if(len>max_allowed_size){
        buffer.clear();
        return false;
    }

    size_t totalSize=sizeof(uint16_t)+sizeof(uint32_t)+len;

    if(buffer.size()<totalSize)return false;

    out.header.type=static_cast<MessageType>(type_raw);
    out.header.length=len;
    out.payload.assign(buffer.begin()+6,buffer.begin()+totalSize);
    buffer.erase(buffer.begin(),buffer.begin()+totalSize);
    return true;
}