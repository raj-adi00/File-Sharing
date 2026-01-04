#pragma once
#include <thread>
#include <atomic>
#include "PeerTable.h"

using namespace std;

class DiscoveryService{
    public:
    DiscoveryService(const string& selfId,int port);
    ~DiscoveryService();

    void start();
    void stop();

    PeerTable& getPeerTable();

    private:
    void run();
    void broadcastLoop();
    void listenLoop();

    string peerId;
    atomic<bool> running;
    int myport;
    
    thread broadcaster;
    thread listener;

    PeerTable peerTable;
};