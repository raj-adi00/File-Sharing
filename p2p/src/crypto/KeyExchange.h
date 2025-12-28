#pragma once
#include <vector>
#include <cstdint>
#include <openssl/evp.h>

using namespace std;

class KeyExchange{
    public:
       KeyExchange();
       ~KeyExchange();

       vector<uint8_t> getPublicKey();
       vector<uint8_t> deriveSharedKey(const vector<uint8_t>& peerPublicKey);
    private:
       EVP_PKEY* keypair;
};