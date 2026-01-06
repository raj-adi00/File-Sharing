    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include "DiscoveryService.h"
    #include "../core/Logger.h"
    #include <chrono>
    #include <thread>
    #include <sstream>
    #include <iostream>

    #define DISCOVERY_MSG_PREFIX "P2P_DISCOVERY::"
    #define DISCOVERY_LEAVE_PREFIX "P2P_LEAVE::"

    using namespace std;

    DiscoveryService::DiscoveryService(const string&selfId,int port,int listenDiscPort,int sendDiscPort):peerId(selfId),running(false),myTcpPort(port),myListenDiscPort(listenDiscPort),mySendDiscPort(sendDiscPort){}

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

        SOCKET sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if(sock==INVALID_SOCKET)return;

        BOOL enable=TRUE;
        setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char*)&enable,sizeof(enable));

        sockaddr_in addr{};
        addr.sin_family=AF_INET;
        addr.sin_port=htons(mySendDiscPort);

        /*activate this setting for testing on remote machine*/
        // addr.sin_addr.s_addr=INADDR_BROADCAST;

        /* activate this setting for testing on localhost*/
        addr.sin_addr.s_addr=inet_addr("127.0.0.1");

        string msg=std::string(DISCOVERY_LEAVE_PREFIX)+peerId;
        sendto(sock,msg.c_str(),(int)msg.size(),0,(sockaddr*)&addr,sizeof(addr));
        closesocket(sock);

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
        addr.sin_port=htons(mySendDiscPort);

        /*activate this setting for testing on remote machine*/
        // addr.sin_addr.s_addr=INADDR_BROADCAST;

        /* activate this setting for testing on localhost*/
        addr.sin_addr.s_addr=inet_addr("127.0.0.1");

        string msg = string(DISCOVERY_MSG_PREFIX) + "|" + peerId + "|" + to_string(myTcpPort);

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
        addr.sin_port=htons(myListenDiscPort);
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
                size_t firstPipe=msg.find('|');
                if(firstPipe==string::npos)continue;
                string content=msg.substr(firstPipe+1);
                stringstream ss(content);
                string remoteId,portStr;

                if(getline(ss,remoteId,'|') && getline(ss,portStr,'|')){
                    if(remoteId==peerId)continue;
                    int remotePort=stoi(portStr);
                    char ipstr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET,&sender.sin_addr,ipstr,sizeof(ipstr));
                    peerTable.addPeer({remoteId,ipstr,remotePort});
                }
            }else if(msg.rfind(DISCOVERY_LEAVE_PREFIX,0)==0){
                string remoteId=msg.substr(strlen(DISCOVERY_LEAVE_PREFIX));
                peerTable.removePeer(remoteId);
                Logger::instance().debug("Peer left: "+remoteId);
            }
        }
        closesocket(sock);
    }

    PeerTable& DiscoveryService::getPeerTable(){
        return peerTable;
    }