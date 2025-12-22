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

int main() {
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
    int max_message_size=config.getInt("max_message_size",10485760);

    Logger::instance().init(logFile);
    Logger::instance().info("P2P Node Starting...");

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa)!=0){
     Logger::instance().error("WSAStartup Failed");
     return 1;
    }
    
    string peerId="peer_"+to_string(rand()%10000);
    Logger::instance().info("PeerId "+peerId);

    PeerCore core(peerId);
    core.start();
    this_thread::sleep_for(chrono::seconds(60));
    core.stop();


    WSACleanup();
    Logger::instance().info("P2P System Shutdown...");

    return 0;
}