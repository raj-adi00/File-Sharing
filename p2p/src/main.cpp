#include <iostream>
#include <string>
#include <winsock2.h>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/PeerCore.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"
#include "protocol/Message.h"
#include "protocol/MessageFramer.h"
#include "protocol/ProtocolSession.h"

using namespace std;

int main(int argc, char* argv[]) {

    if(argc<2)return 0;
    string mode=argv[1];

    srand(static_cast<unsigned int>(time(0)));
    ConfigManager config;
    if (!config.load("config.ini")) {
        cout << "Failed to load config\n";
        return 1;
    }

    string logFile = config.getString("log_file", "default.log");
    int port = config.getInt("port");
    bool debug = config.getBool("debug");
    string server_ip=config.getString("server_ip","127.0.0.1");
    uint32_t max_message_size=config.getInt("max_message_size",10485760);
    uint32_t chunk_size=config.getInt("chunk_size",1048576);

    Logger::instance().init(logFile);
    Logger::instance().info("P2P Node Starting ...");

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    if(mode=="server"){
        TcpListener listener;
        listener.start(port);
       
        TcpConnection conn=listener.acceptClient();
        if(conn.getSocket()==INVALID_SOCKET){
            Logger::instance().error("Failed to accept client connection");
        }else{
            Logger::instance().info("Client connected to Server.");
        }
        string id_server="SERVER";
        ProtocolSession session(std::move(conn),id_server,max_message_size,chunk_size);

        if(session.performServerHandshake()){
            if(session.performKeyExchange()){
               Logger::instance().info("Security established. Ready to receive File.");
               string targetPath = "D:/Projects/FileSharing/p2p/src/data/received_Des.png";
               if(session.recvFile(targetPath)) {
                Logger::instance().info("SUCCESS: File received fully.");
            } else {
                Logger::instance().debug("TRANSFER STOPPED: Check for .resume file to test resume.");
            }
        }
    }
}
    if(mode=="client"){
        TcpConnection conn;
        conn.connectTo(server_ip,port);
        Logger::instance().info("Connected to Server.");
        string id_client="CLIENT";
        ProtocolSession session(std::move(conn),id_client,max_message_size,chunk_size);

        if(session.performClientHandshake()){
            if(session.performKeyExchange()){
                Logger::instance().info("Security established. Ready to send File.");
                  string sourceFile = "D:/Projects/FileSharing/p2p/src/data/Des.png";
                  string metaFile = "D:/Projects/FileSharing/p2p/src/data/file.meta";

                  if(session.sendFile(sourceFile, metaFile)) {
                    Logger::instance().info("SUCCESS: File sent fully.");
                } else {
                    Logger::instance().debug("TRANSFER INTERRUPTED: Ready for resume test.");
                }
            }
        }
    }

    Logger::instance().info("P2P System Shutdown...");
    Sleep(1000);
    WSACleanup();
    return 0;
}