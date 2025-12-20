#include "PeerCore.h"
#include "Logger.h"

PeerCore::PeerCore(){
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