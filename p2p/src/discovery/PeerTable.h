#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

using namespace std;

struct PeerInfo{
    string peerId;
    string ip;
};

class PeerTable{
    public:
    void addPeer(const PeerInfo&peer);
    void printPeers();
    
    private:
    unordered_map<string,PeerInfo> peers;
    mutex mtx;  
};