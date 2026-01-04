#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include "../ui/CLI.h"

using namespace std;

struct PeerInfo{
    string peerId;
    string ip;
};

class PeerTable{
    public:
    void addPeer(const PeerInfo&peer);
    void printPeers();
    void removePeer(const string& peerId);

    vector<PeerDisplay> getPeerList();
    
    private:
    unordered_map<string,PeerInfo> peers;
    mutex mtx;  
};