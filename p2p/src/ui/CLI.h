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
    int selectAction();
    bool confirmFileReceive(const string& peerId,const string& fileName,const string& fileSize);
    string askFilePath();
};