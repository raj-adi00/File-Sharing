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

using namespace std;

int main(int argc, char* argv[]) {
    ConfigManager config;
    if (!config.load("config.ini")) {
        cout << "Failed to load config\n";
        return 1;
    }

    string logFile = config.getString("log_file", "default.log");
    int port = config.getInt("port");
    bool debug = config.getBool("debug");
    string server_ip=config.getString("server_ip","127.0.0.1");
    int max_message_size=config.getInt("max_message_size",10485760);

    Logger::instance().init(logFile);
    Logger::instance().info("P2P System Starting...");
    Logger::instance().info("Debug Mode: " + string(debug ? "true" : "false"));

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa)!=0){
     Logger::instance().error("WSAStartup Failed");
     return 1;
    }

    if(argc<2){
        cout<<"Usage:"<<endl;
        cout<<"  Server:p2p Server"<<endl;
        cout<<"  Client:p2p Client"<<endl;
        return 0;
    }
    
    string mode=argv[1];
    
    if(mode=="Server"){
        Logger::instance().info("Starting Server at port "+to_string(port)+"...");
        TcpListener server;
        if(server.start(port)){
            Logger::instance().info("Server started at port "+to_string(port)+"....");
            TcpConnection client=server.acceptClient();
            Logger::instance().info("Client Connected...");

            vector<uint8_t> buffer;
            Message msg;
            bool messageReceived=false;

            while(!messageReceived){
                int bytes=client.recvBytes(buffer);
                if(bytes<=0)break;
                if(MessageFramer::decode(buffer,msg,max_message_size)){
                    cout<<"Client Echo: "<<static_cast<int>(msg.header.type)<<endl;
                    messageReceived=true;
                }
            }
        }else{
            Logger::instance().error("Failed to start server");
        }
    }else if(mode=="Client"){
        Logger::instance().info("Connecting to: "+server_ip+" at port "+to_string(port)+"...");
        TcpConnection conn;
        if(conn.connectTo(server_ip,port)){
            Logger::instance().info("Connected to: "+server_ip+" at port "+to_string(port)+"...");

            Message hello;
            hello.header.type=MessageType::MSG_HELLO;
            hello.header.length=0;

            auto encoded=MessageFramer::encode(hello,max_message_size);
            conn.sendBytes(encoded);
        }else{
            Logger::instance().error("Failed to connect to: "+server_ip+" at port "+to_string(port)+"...");
        }
    }
    WSACleanup();
    Logger::instance().info("P2P System Shutdown...");

    return 0;
}