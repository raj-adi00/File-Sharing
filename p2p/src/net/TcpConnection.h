#pragma once
#include <string>
#include <winsock2.h>
#include <vector>
#include <cstdint>

using namespace std;

class TcpConnection{
    public:
        TcpConnection();
        explicit TcpConnection(SOCKET s);
        ~TcpConnection();

        bool connectTo(const string &ip,int port);
        int sendData(const char*buffer,int size);
        int recvData(char*buffer,int size);

        int sendBytes(const vector<uint8_t>&data);
        int recvBytes(vector<uint8_t>&buffer);
        void close();
        
    private:
        SOCKET sock;
};