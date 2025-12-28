#pragma once
#include <vector>
#include <cstdint>

using namespace std;

class CryptoEngine{

public:
   CryptoEngine();

   vector<uint8_t> encrypt(const vector<uint8_t>& plain);
   vector<uint8_t> decrypt(const vector<uint8_t>& cipher);

private:
   vector<uint8_t> key;
   vector<uint8_t> iv;
};