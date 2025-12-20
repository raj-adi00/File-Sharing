#pragma once
#include <winsock2.h>
#include "TcpConnection.h"

class TcpListener{
    public:
        TcpListener();
        ~TcpListener();

        bool start(int port);
        TcpConnection acceptClient();
        void stop();
    
    private:
        SOCKET listenSock;
};