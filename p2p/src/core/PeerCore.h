#pragma once
#include "../discovery/DiscoveryService.h"

class PeerCore{
    public:
    PeerCore(const string&peerId);
    ~PeerCore();

    void start();
    void stop();

    private:
    DiscoveryService discovery;
};