#include "CLI.h"
#include<iostream>

using namespace std;

void CLI::showWelcome(){
    cout<<"\n===================================================================================\n";
    cout<<"                         P2P File Transfer System                                    \n";
    cout<<"===================================================================================\n\n";
}   

int CLI::selectPeer(const vector<PeerDisplay>&peers){
    if(peers.empty()){
        cout<<"No Peers discovered yet. \n";
        return -1;
    }

    cout<<"\nAvailable Peers:\n";
    for(size_t i=0;i<peers.size();i++){
        cout<<i+1<<". "<<peers[i].peerId<<" : "<<peers[i].ip<<"\n";
    }

    cout<<"Select Peer (0 to cancel): ";
    int c;
    cin>>c;
    if(c<=0 || c>(int)peers.size()){
        cout<<"Invalid Choice\n";
        return -1;
    }
    return c-1;
}

int CLI::selectAction(){
    cout<<"\n1. Send File\n";
    cout<<"2. Receive File\n";
    cout<<"Choice: ";
    int c;
    cin>>c;
    if(c!=1 && c!=2){
        cout<<"Invalid Choice\n";
        return selectAction();
    }
    return c;       
}

bool CLI::confirmFileReceive(const string& peerId,const string& fileName,const string& fileSize){
    cout<<"\nIncoming file request\n";
    cout<<"From: "<<peerId<<"\n";
    cout<<"File Name: "<<fileName<<"\n";
    cout<<"File Size: "<<fileSize<<"bytes\n";
    cout<<"Accept (y/n): ";
    char c;
    cin>>c;
    return (c=='y' || c=='Y');
}

string CLI::askFilePath(){
    cout<<"Enter File Path: ";
    string path;
    cin>>path;
    return path;
}