#include "ResumeState.h"
#include <fstream>
#include <filesystem>

using namespace std;

ResumeState::ResumeState(size_t total):bitmap(total,0){}

void ResumeState::markReceived(size_t idx){
    if(idx<bitmap.size())bitmap[idx]=1;
}

bool ResumeState::isReceived(size_t idx)const{
    return idx<bitmap.size() && bitmap[idx];
}

vector<uint8_t> ResumeState::getBitmap()const{
    return bitmap;
}

bool ResumeState::save(const string& path){
    ofstream out(path,ios::binary);
    if(!out)return false;
    out.write((char*)bitmap.data(),bitmap.size());
    return true;
}

bool ResumeState::load(const std::string& path){
    if(!std::filesystem::exists(path))return false;
    ifstream in(path,ios::binary);
    if(!in)return false;
    in.read((char*)bitmap.data(),bitmap.size());
    return true;
}