#pragma once
#include<string>
#include<vector>

#include "Message.h"
#include "../net/TcpConnection.h"

class ProtocolSession{
    public:
        ProtocolSession(TcpConnection &&conn,const std::string&peerId,uint32_t max_allowed_size);

        bool performClientHandshake();
        bool performServerHandshake();

        std::string getRemotePeerId() const;
        uint32_t max_allowed_size=1024*1024*10; //10MB
    private:
        bool sendHello();
        bool recvHello(Message &out);
        bool sendHelloAck();
        bool recvHelloAck(Message &out);

        TcpConnection connection;
        std::string selfPeerId;
        std::string remotePeerId;


        static constexpr uint16_t PROTOCOL_VERSION=1;
};