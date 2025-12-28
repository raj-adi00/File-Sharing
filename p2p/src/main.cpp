// #include <iostream>
// #include <string>
// #include <winsock2.h>
// #include "core/Logger.h"
// #include "core/ConfigManager.h"
// #include "core/PeerCore.h"
// #include "net/TcpListener.h"
// #include "net/TcpConnection.h"
// #include "protocol/Message.h"
// #include "protocol/MessageFramer.h"
// #include "protocol/ProtocolSession.h"

// using namespace std;

// int main(int argc, char* argv[]) {

//     if(argc<2)return 0;
//     string mode=argv[1];

//     srand(static_cast<unsigned int>(time(0)));
//     ConfigManager config;
//     if (!config.load("config.ini")) {
//         cout << "Failed to load config\n";
//         return 1;
//     }

//     string logFile = config.getString("log_file", "default.log");
//     int port = config.getInt("port");
//     bool debug = config.getBool("debug");
//     string server_ip=config.getString("server_ip","127.0.0.1");
//     int max_message_size=config.getInt("max_message_size",10485760);

//     Logger::instance().init(logFile);
//     Logger::instance().info("P2P Node Starting...");

//     WSADATA wsa;
//     WSAStartup(MAKEWORD(2,2),&wsa);

//     if(mode=="server"){
//         TcpListener listener;
//         listener.start(port);

//         TcpConnection conn=listener.acceptClient();
//         ProtocolSession session(move(conn),"SERVER",max_message_size);

//         if(session.performServerHandshake()){
//             Message msg;
//             if(session.recvEncryptedMessage(msg)){
//                 string receivedText(msg.payload.begin(),msg.payload.end());
//                 Logger::instance().info("Received: "+receivedText);
//             }else{
//                 Logger::instance().error("Failed to receive message");
//             }
//         }
//     }
//     if(mode=="client"){
//         TcpConnection conn;
//         conn.connectTo(server_ip,port);

//         ProtocolSession session(move(conn),"CLIENT",max_message_size);
//         if(session.performClientHandshake()){
//             Message msg;
//             msg.header.type=MessageType::MSG_PING;
//             string txt="Hello World";
//             msg.payload.assign(txt.begin(),txt.end());
//             session.sendEncryptedMessage(msg);
//         }
//     }

//     Logger::instance().info("P2P System Shutdown...");
//     WSACleanup();
//     return 0;
// }
#include "storage/ChunkManager.h"
#include <iostream>

int main() {
    // Note: Use forward slashes (/) in C++ strings to avoid escape character issues
    std::string inputPath = "D:/Projects/FileSharing/p2p/src/data/file.dat";
    std::string metaPath  = "D:/Projects/FileSharing/p2p/src/data/file.meta";

    ChunkManager cm(inputPath, 1024 * 1024); // 1 MB
    
    if (cm.generateManifest(metaPath)) {
        std::cout << "Manifest generated at: " << metaPath << std::endl;
    } else {
        std::cerr << "Failed to find or read: " << inputPath << std::endl;
    }
    return 0;
}