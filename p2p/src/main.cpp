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
//     uint32_t max_message_size=config.getInt("max_message_size",10485760);
//     uint32_t chunk_size=config.getInt("chunk_size",1048576);

//     Logger::instance().init(logFile);
//     Logger::instance().info("P2P Node Starting ...");

//     WSADATA wsa;
//     WSAStartup(MAKEWORD(2,2),&wsa);

//     if(mode=="server"){
//         TcpListener listener;
//         listener.start(port);
       
//         TcpConnection conn=listener.acceptClient();
//         if(conn.getSocket()==INVALID_SOCKET){
//             Logger::instance().error("Failed to accept client connection");
//         }else{
//             Logger::instance().info("Client connected to Server.");
//         }
//         string id_server="SERVER";
//         ProtocolSession session(std::move(conn),id_server,max_message_size,chunk_size);

//         if(session.performServerHandshake()){
//             if(session.performKeyExchange()){
//                Logger::instance().info("Security established. Ready to receive File.");
//                string targetPath = "D:/Projects/FileSharing/p2p/src/data/received_vid.mp4";
//                if(session.recvFile(targetPath)) {
//                 cout<<endl;
//                 Logger::instance().info("SUCCESS: File received fully.");
//             } else {
//                 Logger::instance().debug("TRANSFER STOPPED: Check for .resume file to test resume.");
//             }
//         }
//     }
// }
//     if(mode=="client"){
//         TcpConnection conn;
//         conn.connectTo(server_ip,port);
//         Logger::instance().info("Connected to Server.");
//         string id_client="CLIENT";
//         ProtocolSession session(std::move(conn),id_client,max_message_size,chunk_size);

//         if(session.performClientHandshake()){
//             if(session.performKeyExchange()){
//                 Logger::instance().info("Security established. Ready to send File.");
//                   string sourceFile = "D:/Projects/FileSharing/p2p/src/data/vid.mp4";
//                   string metaFile = "D:/Projects/FileSharing/p2p/src/data/file.meta";

//                   if(session.sendFile(sourceFile, metaFile)) {
//                     cout<<endl;
//                     Logger::instance().info("SUCCESS: File sent fully.");
//                 } else {
//                     Logger::instance().debug("TRANSFER INTERRUPTED: Ready for resume test.");
//                 }
//             }
//         }
//     }

//     Logger::instance().info("P2P System Shutdown...");
//     Sleep(1000);
//     WSACleanup();
//     return 0;
// }

// #include <iostream>
// #include <string>
// #include <vector>
// #include <thread>
// #include <atomic>
// #include <winsock2.h>
// #include <sstream>

// #include "core/Logger.h"
// #include "core/ConfigManager.h"
// #include "discovery/DiscoveryService.h"
// #include "protocol/ProtocolSession.h"
// #include "net/TcpListener.h"
// #include "ui/CLI.h"

// using namespace std;

// atomic<bool> g_keepListening(true);
// string g_myId;
// uint32_t g_maxMsg, g_chunkSize;
// int g_port;

// // BACKGROUND TASK: Handles incoming connection requests
// void backgroundListenerTask(CLI* cli) {
//     TcpListener listener;
//     if (!listener.start(g_port)) return;

//     while (g_keepListening) {
//         TcpConnection conn = listener.acceptClient();
//         if (conn.getSocket() != INVALID_SOCKET) {
//             ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
            
//             // STAGE 1: Connection Handshake
//             if (session.performServerHandshake()) {
//                 // ASK: Do we want to talk to this ID?
//                 cout << "\n[!] Connection request from " << session.getRemotePeerId() << ". Allow? (y/n): ";
//                 char choice; cin >> choice;

//                 if (choice == 'y' || choice == 'Y') {
//                     if (session.performKeyExchange()) {
//                         // STAGE 2: File Transfer (Now includes file-specific prompt)
//                         session.recvFile(cli); 
//                     }
//                 }
//             }
//         }
//     }
// }

// int main(int argc, char* argv[]) {
//     srand(static_cast<unsigned int>(time(0)));
//     string configFile = "config.ini";
//     if (argc > 1) {
//         configFile = argv[1]; // Use the file passed in terminal
//     }

//     ConfigManager config;
//     if (!config.load(configFile)) return 1;

//     g_myId = "Peer_" + to_string(rand() % 1000);
//     g_port = config.getInt("port", 9000);
//     g_maxMsg = config.getInt("max_message_size", 10485760);
//     g_chunkSize = config.getInt("chunk_size", 65536);

//     Logger::instance().init(config.getString("log_file", "p2p.log"));
//     WSADATA wsa;
//     WSAStartup(MAKEWORD(2, 2), &wsa);

//     DiscoveryService discovery(g_myId);
//     discovery.start();

//     CLI cli;
//     cli.showWelcome();

//     thread bgListener(backgroundListenerTask, &cli);
//     bgListener.detach(); 

//     cout << "Discovery: Scanning for peers... "<<endl;
//     for(int i=0;i<5;i++){
//         cout<<".";
//         this_thread::sleep_for(chrono::seconds(1));
//     }
//     cout<<endl;
//     bool fileSent=false;

//     while (true) {
//         auto peers = discovery.getPeerTable().getPeerList();
//         int idx = cli.selectPeer(peers);
//         if (idx == -1) continue;

//         // SENDER SIDE
//         TcpConnection conn;
//         cout << "[*] Requesting connection to " << peers[idx].peerId << "..." << endl;
        
//         if (conn.connectTo(peers[idx].ip, g_port)) {
//             ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
            
//             // Send our Hello
//             if (session.performClientHandshake()) {
//                 // Here the sender waits. If the receiver says 'n' to Consent 1, 
//                 // performKeyExchange will fail because the receiver closes the socket.
//                 if (session.performKeyExchange()) {
//                     string path = cli.askFilePath();
//                     if(session.sendFile(path, path + ".meta")){
//                         cout << "[*] File sent successfully." << endl;
//                         fileSent = true;
//                         break;
//                     }else{
//                         cout << "[*] File transfer interrupted." << endl;
//                     }
//                 } else {
//                     cout << "[!] Connection was rejected by the remote peer." << endl;
//                 }
//             }
//         } else {
//             cout << "[ERROR] Peer is offline or rejected connection." << endl;
//         }

//         if (fileSent)break;
        
//         cout << "\nPress Enter to continue...";
//         cin.ignore(); cin.get();
//     }
    
//     Logger::instance().info("P2P System Shutdown...");
//     Sleep(1000);
//     WSACleanup();
//     return 0;
// }

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <sstream>

#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "discovery/DiscoveryService.h"
#include "protocol/ProtocolSession.h"
#include "net/TcpListener.h"
#include "ui/CLI.h"

// Do not put 'using namespace std' before includes to avoid 'byte' ambiguity
using namespace std;

atomic<bool> g_keepListening(true);
string g_myId;
uint32_t g_maxMsg, g_chunkSize;
int g_port;

void backgroundListenerTask(CLI* cli) {
    TcpListener listener;
    // We use the global g_port which will be 9000 or 9001
    if (!listener.start(g_port)) return;

    while (g_keepListening) {
        TcpConnection conn = listener.acceptClient();
        if (conn.getSocket() != INVALID_SOCKET) {
            ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
            
            if (session.performServerHandshake()) {
                cout << "\n[!] Connection request from " << session.getRemotePeerId() << ". Allow? (y/n): ";
                char choice; cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    if (session.performKeyExchange()) {
                        if(session.recvFile(cli)) {
                            cout << "\n[*] File received successfully." << endl;
                        }else{
                            cout << "\n[*] File transfer interrupted." << endl;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));
    
    // 1. CONFIG LOADING
    ConfigManager config;
    config.load("config.ini"); 

    g_myId = "Peer_" + to_string(rand() % 1000);
    g_maxMsg = config.getInt("max_message_size", 10485760);
    g_chunkSize = config.getInt("chunk_size", 65536);
    
    // 2. TESTING LOGIC FOR ONE LAPTOP
    g_port = config.getInt("port", 9000);
    bool isReceiverOnly = false;

    if (argc > 1 && string(argv[1]) == "receiver") {
        g_port = 9001; // Force Terminal 2 to use a different door
        isReceiverOnly = true;
    }

    Logger::instance().init(config.getString("log_file", "p2p.log"));
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    DiscoveryService discovery(g_myId);
    discovery.start();

    CLI cli;
    cli.showWelcome();
    cout << "--- NODE: " << g_myId << " | PORT: " << g_port << " ---" << endl;

    thread bgListener(backgroundListenerTask, &cli);
    bgListener.detach(); 

    if (isReceiverOnly) {
        cout << "\n[*] RECEIVER MODE ACTIVE. Waiting for incoming files..." << endl;
        while(true) { Sleep(1000); } // Keep receiver alive
    }

    // 3. SENDER LOOP
    while (true) {
        auto allPeers = discovery.getPeerTable().getPeerList();
        
        // Filter: Don't show ourselves in the list
        vector<PeerDisplay> others;
        for(auto& p : allPeers) {
            if(p.peerId != g_myId) others.push_back(p);
        }

        if (others.empty()) {
            cout << "\rScanning for peers (Open another terminal and run: p2p.exe receiver)..." << flush;
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }

        int idx = cli.selectPeer(others);
        if (idx == -1) continue;

        TcpConnection conn;
        cout << "[*] Requesting connection to " << others[idx].peerId << "..." << endl;
        
        // Try port 9001 first (for the receiver terminal), then fallback to 9000
        bool connected = conn.connectTo(others[idx].ip, 9001);
        if(!connected) connected = conn.connectTo(others[idx].ip, 9000);

        if (connected) {
            ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
            if (session.performClientHandshake()) {
                if (session.performKeyExchange()) {
                    string path = cli.askFilePath();
                    if(session.sendFile(path, path + ".meta")){
                        cout << "[*] File sent successfully." << endl;
                        break; 
                    }
                }
            }
        } else {
            cout << "[ERROR] Could not connect to peer." << endl;
        }
    }
    
    WSACleanup();
    return 0;
}