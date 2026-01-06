#pragma once
#include<vector>
#include<string>

using namespace std;

struct PeerDisplay{
    string peerId;
    string ip;
    int port;
};

class CLI{
public:
    void showWelcome();
    int selectPeer(const vector<PeerDisplay>& peers);
    bool confirmFileReceive(const string& peerId,const string& fileName,const string& fileSize);
    int selectAction();
    string askFilePath();
};