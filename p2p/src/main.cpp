#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <sstream>

#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "discovery/DiscoveryService.h"
#include "core/PeerCore.h"
#include "protocol/ProtocolSession.h"
#include "net/TcpListener.h"
#include "ui/CLI.h"

using namespace std;

atomic<bool> g_keepListening(true);
string g_myId;
uint32_t g_maxMsg, g_chunkSize;
int g_port,g_sendPort,g_listenPort;

void backgroundListenerTask(CLI* cli) {
    TcpListener listener;
    if (!listener.start(g_port)) return;

    while (g_keepListening) {
        TcpConnection conn = listener.acceptClient();
        if (conn.getSocket() != INVALID_SOCKET) {
            ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
            
            // STAGE 1: Connection Handshake
            if (session.performServerHandshake()) {
                // ASK: Do we want to talk to this ID?
                cout << "\n[!] Connection request from " << session.getRemotePeerId() << ". Allow? (y/n): ";
                char choice; cin >> choice;

                if (choice == 'y' || choice == 'Y') {
                    if (session.performKeyExchange()) {
                        // STAGE 2: File Transfer (Now includes file-specific prompt)
                        session.recvFile(cli); 
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));
    string configFile = "config.ini";
    if (argc > 1) {
        configFile = argv[1];
    }

    ConfigManager config;
    if (!config.load(configFile)) return 1;

    g_myId = "Peer_" + to_string(rand() % 1000);
    g_port = config.getInt("port", 9000);
    g_maxMsg = config.getInt("max_message_size", 10485760);
    g_chunkSize = config.getInt("chunk_size", 65536);
    g_sendPort = config.getInt("send_port", 5001);
    g_listenPort = config.getInt("listen_port", 5000);

    if(argc==1)g_sendPort=g_listenPort;

    Logger::instance().init(config.getString("log_file", "p2p.log"));
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    PeerCore core(g_myId, g_port, g_listenPort, g_sendPort);
    core.start();

    CLI cli;
    cli.showWelcome();
    cout<<"--- "<<g_myId<<" Listening on TCP "<<g_port<<" ---"<<endl;
    int action=cli.selectAction();
    if(action==3){
        core.stop();
        WSACleanup();
        return 0;
    }
    thread bgListener(backgroundListenerTask, &cli);
    bgListener.detach(); 

    if(action==2){
        cout<<"[*] Receiver Mode Active. Waiting for incoming files...\n";
        cout<<"[*] Discovery is broadcasting your present to others.\n";
        while(true){
            this_thread::sleep_for(chrono::seconds(1));
        }
    }else{
        cout << "Discovery: Scanning for peers... "<<endl;
        for(int i=0;i<5;i++){
            cout<<"."<<flush;
            this_thread::sleep_for(chrono::seconds(1));
        }
        cout<<endl;
        bool fileSent=false;
        
        while (true) {
            auto peers = core.getPeerList();
            int idx = cli.selectPeer(peers);
            if (idx == -1) continue;
            if (idx == -2) break;
            
            // SENDER SIDE
            TcpConnection conn;
            cout << "[*] Requesting connection to " << peers[idx].peerId << "..." << endl;
        
            if (conn.connectTo(peers[idx].ip, peers[idx].port)) {
                ProtocolSession session(std::move(conn), g_myId, g_maxMsg, g_chunkSize);
                // Send our Hello
                if (session.performClientHandshake()) {
                    if (session.performKeyExchange()) {
                        string path = cli.askFilePath();
                        if(session.sendFile(path, path + ".meta")){
                            cout << "[*] File sent successfully." << endl;
                            fileSent = true;
                            break;
                        }else{
                            cout << "[*] File transfer interrupted." << endl;
                        }
                    } else {
                        cout << "[!] Connection was rejected by the remote peer." << endl;
                    }
                }
            } else {
                cout << "[ERROR] Peer is offline or rejected connection." << endl;
            }
            if (fileSent)break;
            cout << "\nPress Enter to continue...";
            cin.ignore(); cin.get();
        }
    }
    Logger::instance().info("P2P System Shutdown...");
    Sleep(1000);
    WSACleanup();
    return 0;
}