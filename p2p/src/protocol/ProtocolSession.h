#pragma once
#include<string>
#include<vector>

#include "Message.h"
#include "../net/TcpConnection.h"
#include "../crypto/CryptoEngine.h"
#include "../crypto/KeyExchange.h"
#include "../storage/ResumeState.h"

class ProtocolSession{
    public:
        ProtocolSession(TcpConnection &&conn, const std::string& peerId, uint32_t max_size, uint32_t chunksz);

        bool performClientHandshake();
        bool performServerHandshake();

        bool sendEncryptedMessage(Message &msg);
        bool recvEncryptedMessage(Message &out);

        bool performKeyExchange();

        bool sendFile(const std::string&filePath,const string&metaPath);
        bool recvFile(const std::string&outputPath);

        bool handleResumeAsSender(size_t totalChunks,std::vector<uint8_t>& outRemoteBitmap);
        bool handlResumeAsReceiver(ResumeState &state);

        std::string getRemotePeerId() const;
        uint32_t max_allowed_size=1024*1024*10; //10MB

        ~ProtocolSession();
    private:
        bool sendHello();
        bool recvHello(Message &out);
        bool sendHelloAck();
        bool recvHelloAck(Message &out);

        TcpConnection connection;
        std::string selfPeerId;
        std::string remotePeerId;
        KeyExchange keyEx;
        CryptoEngine* crypto=nullptr;
        uint32_t chunkSize;

        static constexpr uint16_t PROTOCOL_VERSION=1;
};