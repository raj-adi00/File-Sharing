#pragma once
#include<string>
#include<vector>
#include<cstdint>

using namespace std;

class ChunkManager{
    public:
       ChunkManager(const string& filePath,size_t chunkSize);
       bool generateManifest(const string & metaPath);
       vector<string> parseManifestData(const vector<uint8_t>& manifestData);
       string sha256(const vector<uint8_t>& data);
    private:
       string filePath;
       size_t chunkSize;
};