#pragma once
#include "../discovery/DiscoveryService.h"

class PeerCore{
    public:
    PeerCore();
    ~PeerCore();

    void start();
    void stop();

    private:
    DiscoveryService discovery;
};