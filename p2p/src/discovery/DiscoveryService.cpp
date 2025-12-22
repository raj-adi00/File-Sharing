#include <winsock2.h>
#include <ws2tcpip.h>
#include "DiscoveryService.h"
#include "../core/Logger.h"
#include <chrono>
#include <thread>

#define DISCOVERY_PORT 50000
#define DISCOVERY_MSG_PREFIX "P2P_DISCOVERY::"

using namespace std;

DiscoveryService::DiscoveryService(const string&selfId):peerId(selfId),running(false){}

DiscoveryService::~DiscoveryService(){
    stop();
}

void DiscoveryService::start(){
    if(running)return;
    
    running=true;
    broadcaster=thread(&DiscoveryService::broadcastLoop,this);
    listener=thread(&DiscoveryService::listenLoop,this);

    Logger::instance().info("Discovery Service started(UDP)");
}

void DiscoveryService::stop(){
    if(!running)return;

    running=false;
    if(broadcaster.joinable())broadcaster.join();
    if(listener.joinable())listener.join();

    Logger::instance().info("Discovery Service stopped");
}

void DiscoveryService::run(){
    while(running){
        Logger::instance().debug("Discovery Service running...");
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void DiscoveryService::broadcastLoop(){
    SOCKET sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sock==INVALID_SOCKET)return;

    BOOL enable=TRUE;
    setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char*)&enable,sizeof(enable));

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(DISCOVERY_PORT);
    addr.sin_addr.s_addr=INADDR_BROADCAST;

    string msg=DISCOVERY_MSG_PREFIX+peerId;

    while(running){
        sendto(sock,msg.c_str(),(int)msg.size(),0,(sockaddr*)&addr,sizeof(addr));
        this_thread::sleep_for(chrono::seconds(2));
    }
    closesocket(sock);
}

void DiscoveryService::listenLoop(){
    SOCKET sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sock==INVALID_SOCKET)return;
    
    BOOL reuse=TRUE;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse));
    
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(DISCOVERY_PORT);
    addr.sin_addr.s_addr=INADDR_ANY;

    bind(sock,(sockaddr*)&addr,sizeof(addr));

    char buffer[512];
    sockaddr_in sender{};
    int senderLen=sizeof(sender);

    while(running){
        int bytes=recvfrom(sock,buffer,sizeof(buffer)-1,0,(sockaddr*)&sender,&senderLen);
        if(bytes<=0)continue;
        buffer[bytes]='\0';
        string msg(buffer);
        if(msg.rfind(DISCOVERY_MSG_PREFIX,0)==0){
            string remoteId=msg.substr(strlen(DISCOVERY_MSG_PREFIX));
            if(remoteId==peerId)continue;

            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET,&sender.sin_addr,ipStr,sizeof(ipStr));

            peerTable.addPeer({remoteId,ipStr});
        }
    }
    closesocket(sock);
}