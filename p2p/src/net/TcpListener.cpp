#include "TcpListener.h"

TcpListener::TcpListener() : listenSock(INVALID_SOCKET){}

TcpListener::~TcpListener(){
    stop();
}

bool TcpListener::start(int port){
    listenSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenSock==INVALID_SOCKET)return false;

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=INADDR_ANY;

    if(bind(listenSock,(sockaddr*)&addr,sizeof(addr))!=0)return false;
    return listen(listenSock,1)==0;
}

TcpConnection TcpListener::acceptClient(){
    SOCKET client=accept(listenSock,nullptr,nullptr);
    return TcpConnection(client);
}

void TcpListener::stop(){
    if(listenSock!=INVALID_SOCKET){
        closesocket(listenSock);
        listenSock=INVALID_SOCKET;
    }
}