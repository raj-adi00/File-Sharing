#pragma once
#include <cstdint>
#include <vector>

using namespace std;

enum class MessageType:uint16_t{
    MSG_HELLO=1,
    MSG_ERROR=2,
    MSG_PING=3,
    MSG_HELLO_ACK=4,
    MSG_KEY_EXCHG=5,
    MSG_KEY_EXCHG_ACK=6,
    MSG_FILE_OFFER=7,   //Sender->Receiver
    MSG_FILE_ACCEPT=8,  //Receiver->Sender
    MSG_CHUNK_DATA=9,   //Sender->Receiver
    MSG_CHUNK_ACK=10,    //Receiver->Sender
    MSG_RESUME_REQUEST=11,
    MSG_RESUME_BITMAP=12,
    MSG_FILE_REJECT=13,
};

struct MessageHeader{
   MessageType type;
   uint32_t length;
};

struct Message{
   MessageHeader header;
   vector<uint8_t> payload;
};