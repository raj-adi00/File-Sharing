#include "FileManager.h"
#include <fstream>

using namespace std;

bool FileManager::readChunk(const string & path,size_t offset,size_t size,vector<uint8_t>& out){
    ifstream file(path,ios::binary);
    if(!file)return false;

    file.seekg(offset);
    out.resize(size);
    file.read(reinterpret_cast<char*>(out.data()),size);
    out.resize(file.gcount());
    return true;
}

size_t FileManager::getFileSize(const string& path){
    ifstream file(path,ios::binary | ios::ate);
    if(!file)return 0;
    return static_cast<size_t>(file.tellg());
}