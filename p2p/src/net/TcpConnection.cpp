#include "TcpConnection.h"
#include <iostream>
#include <cstdint>

using namespace std;

TcpConnection::TcpConnection():sock(INVALID_SOCKET){}

TcpConnection::TcpConnection(SOCKET s):sock(s){}

TcpConnection::~TcpConnection(){
    close();
}

TcpConnection::TcpConnection(TcpConnection &&other)noexcept:sock(other.sock){
    other.sock=INVALID_SOCKET;
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other)noexcept{
    if(this!=&other){
        close();
        this->sock=other.sock;
        other.sock=INVALID_SOCKET;
    }
    return *this;
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

int TcpConnection::sendBytes(const vector<uint8_t>&data){
    // return send(sock,reinterpret_cast<const char*>(data.data()),(int)data.size(),0);
    return sendData(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}

int TcpConnection::recvBytes(vector<uint8_t>&buffer){
    uint8_t temp[1024];
    int bytes=recv(sock,reinterpret_cast<char*>(temp),sizeof(temp),0);
    if(bytes>0){
        buffer.insert(buffer.end(),temp,temp+bytes);
    }
    return bytes;
}

void TcpConnection::close(){
    if(sock!=INVALID_SOCKET){
        closesocket(sock);
        sock=INVALID_SOCKET;
    }
}

SOCKET TcpConnection::getSocket(){
    return sock;
}
