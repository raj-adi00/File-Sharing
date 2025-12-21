#pragma once
#include "Message.h"
#include <vector>

using namespace std;

class MessageFramer{
public:
   static vector<uint8_t> encode(const Message& message,uint32_t max_allowed_size);
   static bool decode(vector<uint8_t>& buffer,Message & out,uint32_t max_allowed_size);
};