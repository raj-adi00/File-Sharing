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
    MSG_FILE_OFFER=7,
    MSG_FILE_ACCEPT=8,
    MSG_CHUNK_DATA=9,
    MSG_CHUNK_ACK=10
};

struct MessageHeader{
   MessageType type;
   uint32_t length;
};

struct Message{
   MessageHeader header;
   vector<uint8_t> payload;
};