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

std::vector<PeerDisplay> PeerTable::getPeerList() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<PeerDisplay> displayList;

    // Iterate through your map of PeerInfo
    for (auto const& [id, info] : peers) {
        PeerDisplay pd;
        pd.peerId = info.peerId;
        pd.ip = info.ip;
        // If PeerDisplay has more fields, map them here
        
        displayList.push_back(pd);
    }

    return displayList;
}

void PeerTable::removePeer(const string& peerId) {
    lock_guard<mutex> lock(mtx);
    if(!peers.count(peerId))return;
    peers.erase(peerId);
}