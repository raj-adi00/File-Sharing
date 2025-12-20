#pragma once
#include <thread>
#include <atomic>
using namespace std;

class DiscoveryService{
    public:
    DiscoveryService();
    ~DiscoveryService();

    void start();
    void stop();

    private:
    void run();
    
    thread worker;
    atomic<bool> running;
};