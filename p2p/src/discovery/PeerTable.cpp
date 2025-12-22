#include "PeerTable.h"
#include "../core/Logger.h"

using namespace std;

void PeerTable::addPeer(const PeerInfo& peer) {
    lock_guard<mutex> lock(mtx);
    
    if(peers.count(peer.peerId))return;
    
    peers[peer.peerId] = peer;
    Logger::instance().info("Discovered Peer: "+peer.peerId +" @ "+peer.ip);
}

void PeerTable::printPeers(){
    lock_guard<mutex> lock(mtx);
    Logger::instance().info("-----Known Peers-----");
    for(auto &[id,peer]:peers){
        Logger::instance().info(id+" -> "+peer.ip);
    }
}