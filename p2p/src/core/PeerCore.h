#pragma once
#include "../discovery/DiscoveryService.h"

class PeerCore{
    public:
    PeerCore(const string&peerId,int tcpPort,int listenDiscPort,int sendDiscPort);
    ~PeerCore();

    void start();
    void stop();
    vector<PeerDisplay> getPeerList();

    private:
    DiscoveryService discovery;
};