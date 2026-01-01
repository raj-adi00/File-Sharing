#pragma once
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

class ResumeState{
    public:
        ResumeState(size_t totalChunks);
        void markReceived(size_t idx);
        bool isReceived(size_t idx) const;

        vector<uint8_t> getBitmap() const;

        bool save(const string& path);
        bool load(const string& path);
    private:
        vector<uint8_t> bitmap;  //1=received, 0=not received
};