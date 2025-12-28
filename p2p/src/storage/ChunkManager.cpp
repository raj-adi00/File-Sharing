#include "ChunkManager.h"
#include "FileManager.h"

#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

ChunkManager::ChunkManager(const string&path,size_t csize):filePath(path),chunkSize(csize){}

string ChunkManager::sha256(const vector<uint8_t>& data){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data.data(),data.size(),hash);

    ostringstream oss;
    for(int i=0;i<SHA256_DIGEST_LENGTH;i++)
        oss<<hex<<setw(2)<<setfill('0')<<(int)hash[i];
    return oss.str();
}

bool ChunkManager::generateManifest(const string & metaPath){
    size_t fileSize=FileManager::getFileSize(filePath);
    if(fileSize==0)return false;

    size_t totalChunks=(fileSize+chunkSize-1)/chunkSize;
    ofstream meta(metaPath);
    if(!meta)return false;

    meta<<"FILE NAME="<<filePath<<"\n";
    meta<<"FILE SIZE="<<fileSize<<"\n";
    meta<<"CHUNK SIZE="<<chunkSize<<"\n";
    meta<<"TOTAL CHUNKS="<<totalChunks<<"\n\n";

    for(size_t i=0;i<totalChunks;i++){
        size_t offset=i*chunkSize;
        size_t size=min(chunkSize,fileSize-offset);
        vector<uint8_t> buffer;
        FileManager::readChunk(filePath,offset,size,buffer);

        string hash=sha256(buffer);
        meta<<"CHUNK_"<<i<<" HASH="<<hash<<"\n";
    }
    return true;
}