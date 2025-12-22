#pragma once
#include <thread>
#include <atomic>
#include "PeerTable.h"

using namespace std;

class DiscoveryService{
    public:
    DiscoveryService(const string& selfId);
    ~DiscoveryService();

    void start();
    void stop();

    private:
    void run();
    void broadcastLoop();
    void listenLoop();

    string peerId;
    atomic<bool> running;
    
    thread broadcaster;
    thread listener;

    PeerTable peerTable;
};