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

        TcpConnection(TcpConnection&& other) noexcept;
        TcpConnection& operator=(TcpConnection&& other) noexcept;

        TcpConnection(const TcpConnection&) = delete;
        TcpConnection& operator=(const TcpConnection&) = delete;

        bool connectTo(const string &ip,int port);
        int sendData(const char*buffer,int size);
        int recvData(char*buffer,int size);
        SOCKET getSocket();

        int sendBytes(const vector<uint8_t>&data);
        int recvBytes(vector<uint8_t>&buffer);
        void close();
        
    private:
        SOCKET sock;
};