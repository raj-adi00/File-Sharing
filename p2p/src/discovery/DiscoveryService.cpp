#include "DiscoveryService.h"
#include "../core/Logger.h"
#include <chrono>
#include <thread>

using namespace std;

DiscoveryService::DiscoveryService():running(false){}

DiscoveryService::~DiscoveryService(){
    stop();
}

void DiscoveryService::start(){
    if(running)return;
    running=true;
    worker=thread(DiscoveryService::run,this);

    Logger::instance().info("Discovery Service started");
}

void DiscoveryService::stop(){
    if(!running)return;

    running=false;
    if(worker.joinable())
    worker.join();

    Logger::instance().info("Discovery Service stopped");
}

void DiscoveryService::run(){
    while(running){
        Logger::instance().debug("Discovery Service running...");
        this_thread::sleep_for(chrono::seconds(1));
    }
}