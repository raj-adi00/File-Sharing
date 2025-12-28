#pragma once
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

class FileManager{
    public:
       static bool readChunk(const string &path,size_t offset,size_t size,vector<uint8_t>& out);
       static size_t getFileSize(const string& path);
};