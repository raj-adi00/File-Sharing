#include "TcpConnection.h"
#include <iostream>

TcpConnection::TcpConnection():sock(INVALID_SOCKET){}

TcpConnection::TcpConnection(SOCKET s):sock(s){}

TcpConnection::~TcpConnection(){
    close();
}

bool TcpConnection::connectTo(const string &ip,int port){
    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(sock==INVALID_SOCKET)return false;

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip.c_str());

    return connect(sock,(sockaddr*)&addr,sizeof(addr))==0;
}

int TcpConnection::sendData(const char*buffer,int size){
    int totalSent=0;
    while(totalSent<size){
        int sent=send(sock,buffer+totalSent,size-totalSent,0);
        if(sent==SOCKET_ERROR)return -1;
        totalSent+=sent;
    }
    return totalSent;
}

int TcpConnection::recvData(char*buffer,int size){
    return recv(sock,buffer,size,0);
}

void TcpConnection::close(){
    if(sock!=INVALID_SOCKET){
        closesocket(sock);
        sock=INVALID_SOCKET;
    }
}
