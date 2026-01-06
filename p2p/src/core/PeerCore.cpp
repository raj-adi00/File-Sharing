#include "PeerCore.h"
#include "Logger.h"

PeerCore::PeerCore(const string&peerId,int tcpPort,int listenDiscPort,int sendDiscPort):discovery(peerId,tcpPort,listenDiscPort,sendDiscPort){
    Logger::instance().info("PeerCore created");
}

PeerCore::~PeerCore(){
    stop();
    Logger::instance().info("PeerCore destroyed");
}

void PeerCore::start(){
    Logger::instance().info("PeerCore Starting Services....");
    discovery.start();
}

void PeerCore::stop(){
    Logger::instance().info("PeerCore Stopping Services....");
    discovery.stop();
}

vector<PeerDisplay> PeerCore::getPeerList(){
    return discovery.getPeerTable().getPeerList();
}