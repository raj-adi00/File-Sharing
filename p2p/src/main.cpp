#include <iostream>
#include <string>
#include <winsock2.h>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/PeerCore.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"

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
            char buffer[1024]={0};
            int bytesRecieved=client.recvData(buffer,sizeof(buffer));
            if(bytesRecieved>0){
                cout<<"Client Echo: "<<buffer<<endl;
                client.sendData(buffer,bytesRecieved);
            }
        }else{
            Logger::instance().error("Failed to start server");
        }
    }else if(mode=="Client"){
        Logger::instance().info("Connecting to: "+server_ip+" at port "+to_string(port)+"...");
        TcpConnection conn;
        if(conn.connectTo(server_ip,port)){
            Logger::instance().info("Connected to: "+server_ip+" at port "+to_string(port)+"...");

            string msg="Hello from Client";
            conn.sendData(msg.c_str(),msg.length());
            char buffer[1024]={0};
            int bytesRecieved=conn.recvData(buffer,sizeof(buffer));
            cout<<"Server Echo: "<<buffer<<endl;
        }else{
            Logger::instance().error("Failed to connect to: "+server_ip+" at port "+to_string(port)+"...");
        }
    }
    WSACleanup();
    Logger::instance().info("P2P System Shutdown...");

    return 0;
}