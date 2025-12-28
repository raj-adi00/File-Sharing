#pragma once
#include<string>
#include<vector>
#include<cstdint>

using namespace std;

class ChunkManager{
    public:
       ChunkManager(const string& filePath,size_t chunkSize);
       bool generateManifest(const string & metaPath);
    private:
       string filePath;
       size_t chunkSize;
       string sha256(const vector<uint8_t>& data);
};