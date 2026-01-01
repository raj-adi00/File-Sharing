#include "KeyExchange.h"
#include <openssl/ec.h>
#include <openssl/kdf.h>
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include "../core/Logger.h"

using namespace std;

KeyExchange::KeyExchange() : keypair(nullptr) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) {
        Logger::instance().error("KeyExchange: Failed to create EVP_PKEY_CTX");
        return; 
    }
    
    if (EVP_PKEY_keygen_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0 ||
        EVP_PKEY_keygen(ctx, &keypair) <= 0) 
    {
        Logger::instance().error("KeyExchange: Failed to generate EC keypair");
    }

    EVP_PKEY_CTX_free(ctx);
}

KeyExchange::~KeyExchange() {
    if (keypair) {
        EVP_PKEY_free(keypair);
    }
}

vector<uint8_t> KeyExchange::getPublicKey(){
    int len=i2d_PUBKEY(keypair,nullptr);
    vector<uint8_t> buf(len);
    unsigned char* p=buf.data();
    i2d_PUBKEY(keypair,&p);
    return buf;
}

vector<uint8_t> KeyExchange::deriveSharedKey(const vector<uint8_t>& peerPublicKey){
    const unsigned char* p=peerPublicKey.data();
    EVP_PKEY* peerKey=d2i_PUBKEY(nullptr,&p,peerPublicKey.size());

    if(!peerKey){
        Logger::instance().error("KeyExchange: Failed to parse peer public key");
        return {};
    }

    EVP_PKEY_CTX* ctx=EVP_PKEY_CTX_new(keypair,nullptr);
    EVP_PKEY_derive_init(ctx);
    EVP_PKEY_derive_set_peer(ctx,peerKey);
    size_t secretLen;
    EVP_PKEY_derive(ctx,nullptr,&secretLen);
    vector<uint8_t> secret(secretLen);
    EVP_PKEY_derive(ctx,secret.data(),&secretLen);
    EVP_PKEY_free(peerKey);
    EVP_PKEY_CTX_free(ctx);
    secret.resize(secretLen);
    return secret;
}
