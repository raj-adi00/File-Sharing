#include "CryptoEngine.h"
#include <openssl/evp.h>
#include <cstring>

using namespace std;

CryptoEngine::CryptoEngine(){
    const char* k="01234567890123456789012345678901";
    const char* i="0123456789012345";

    key.assign(k,k+32);
    iv.assign(i,i+16);
}

vector<uint8_t>CryptoEngine::encrypt(const vector<uint8_t>& plain){
    EVP_CIPHER_CTX* ctx=EVP_CIPHER_CTX_new();
    vector<uint8_t> cipher(plain.size()+16);
    int len=0,total=0;

    EVP_EncryptInit_ex(ctx,EVP_aes_256_cbc(),nullptr,key.data(),iv.data());
    EVP_EncryptUpdate(ctx,cipher.data(),&len,plain.data(),plain.size());

    total=len;

    EVP_EncryptFinal_ex(ctx,cipher.data()+len,&len);
    total+=len;

    EVP_CIPHER_CTX_free(ctx);
    cipher.resize(total);
    return cipher;
}

vector<uint8_t> CryptoEngine::decrypt(const vector<uint8_t>& cipher){
    EVP_CIPHER_CTX* ctx=EVP_CIPHER_CTX_new();
    vector<uint8_t> plain(cipher.size());
    int len=0,total=0;

    EVP_DecryptInit_ex(ctx,EVP_aes_256_cbc(),nullptr,key.data(),iv.data());
    EVP_DecryptUpdate(ctx,plain.data(),&len,cipher.data(),cipher.size());

    total=len;
    EVP_DecryptFinal_ex(ctx,plain.data()+len,&len);
    total+=len;

    EVP_CIPHER_CTX_free(ctx);
    plain.resize(total);
    return plain;
}