#pragma once
#include <string>
#include <winsock2.h>

using namespace std;

class TcpConnection{
    public:
        TcpConnection();
        explicit TcpConnection(SOCKET s);
        ~TcpConnection();

        bool connectTo(const string &ip,int port);
        int sendData(const char*buffer,int size);
        int recvData(char*buffer,int size);
        void close();
        
    private:
        SOCKET sock;
};