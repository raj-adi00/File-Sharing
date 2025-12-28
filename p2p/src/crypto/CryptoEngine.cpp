#include "CryptoEngine.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <cstring>

using namespace std;

CryptoEngine::CryptoEngine(const vector<uint8_t>&k){
    key.resize(SHA256_DIGEST_LENGTH);
    SHA256(k.data(),k.size(),key.data());
}

vector<uint8_t>CryptoEngine::encrypt(const vector<uint8_t>& plain){
    EVP_CIPHER_CTX* ctx=EVP_CIPHER_CTX_new();

    vector<uint8_t> iv(12);
    RAND_bytes(iv.data(),iv.size());

    vector<uint8_t> out(plain.size()+16);
    int len,total;

    EVP_EncryptInit_ex(ctx,EVP_aes_256_gcm(),nullptr,nullptr,nullptr);
    EVP_EncryptInit_ex(ctx,nullptr,nullptr,key.data(),iv.data());

    EVP_EncryptUpdate(ctx,out.data(),&len,plain.data(),plain.size());
    total=len;

    EVP_EncryptFinal_ex(ctx,out.data()+len,&len);
    total+=len;

    vector<uint8_t> tag(16);
    EVP_CIPHER_CTX_ctrl(ctx,EVP_CTRL_GCM_GET_TAG,16,tag.data());\
    EVP_CIPHER_CTX_free(ctx);

    vector<uint8_t> result;
    result.insert(result.end(),iv.begin(),iv.end());
    result.insert(result.end(),out.begin(),out.begin()+total);
    result.insert(result.end(),tag.begin(),tag.end());
    
    return result;
}

vector<uint8_t> CryptoEngine::decrypt(const vector<uint8_t>& cipher){
    if(cipher.size()<(12+16))return {};
    
    EVP_CIPHER_CTX* ctx=EVP_CIPHER_CTX_new();

    const uint8_t* iv=cipher.data();
    const uint8_t* taga=cipher.data()+cipher.size()-16;
    const uint8_t* enc=cipher.data()+12;
    int encLen=cipher.size()-12-16;

    vector<uint8_t> plain(encLen);
    int len,total;

    EVP_DecryptInit_ex(ctx,EVP_aes_256_gcm(),nullptr,nullptr,nullptr);
    EVP_DecryptInit_ex(ctx,nullptr,nullptr,key.data(),iv);
    
    EVP_DecryptUpdate(ctx,plain.data(),&len,enc,encLen);

    total=len;
    EVP_CIPHER_CTX_ctrl(ctx,EVP_CTRL_GCM_SET_TAG,16,(void*)taga);

    if(EVP_DecryptFinal_ex(ctx,plain.data()+len,&len)<=0){
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    total+=len;
    EVP_CIPHER_CTX_free(ctx);

    plain.resize(total);
    return plain;
}