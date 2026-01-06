#pragma once
#include <thread>
#include <atomic>
#include "PeerTable.h"

using namespace std;

class DiscoveryService{
    public:
    DiscoveryService(const string& selfId,int tcpPort,int listenDiscPort,int sendDiscPort);
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
    int myTcpPort;
    int myListenDiscPort;
    int mySendDiscPort;
    
    thread broadcaster;
    thread listener;

    PeerTable peerTable;
};