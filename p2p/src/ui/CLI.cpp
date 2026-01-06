#include "CLI.h"
#include<iostream>
#include<limits>

using namespace std;

void CLI::showWelcome(){
    cout<<"\n===================================================================================\n";
    cout<<"                         P2P File Transfer System                                    \n";
    cout<<"===================================================================================\n\n";
}   

int CLI::selectPeer(const vector<PeerDisplay>&peers){
    if(peers.empty()){
        cout<<"No Peers discovered yet..."<<flush;
        return -1;
    }

    cout<<"\nAvailable Peers:\n";
    for(size_t i=0;i<peers.size();i++){
        cout<<i+1<<". "<<peers[i].peerId<<" : "<<peers[i].ip<<"\n";
    }

    cout<<"Select Peer (0 to cancel): ";
    int c;
    if(!(cin>>c)){
        cout<<"Invalid Choice\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    if(c==0)return -2;

    if(c<0 || c>(int)peers.size()){
        cout<<"Invalid Choice\n";
        return -1;
    }
    return c-1;
}


bool CLI::confirmFileReceive(const string& peerId,const string& fileName,const string& fileSize){
    cout<<"\n[ALERT]Incoming file request\n";
    cout<<"From: "<<peerId<<"\n";
    cout<<"File Name: "<<fileName<<"\n";
    cout<<"File Size: "<<fileSize<<"bytes\n";
    cout<<"Accept (y/n): ";
    char c;
    cin>>c;

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    cin.clear();

    return (c=='y' || c=='Y');
}

int CLI::selectAction(){
   cout<<"\nSelect Action:\n";
   cout<<"1. Send File\n";
   cout<<"2. Receive File\n";
   cout<<"3. Exit\n";
   int c;
   if(!(cin>>c)){
       cout<<"Please Confirm your action\n";
       cin.clear();
       cin.ignore(numeric_limits<streamsize>::max(), '\n');
       return -1;
   } 
   if(c<1 || c>3){
       cout<<"Please Confirm your action\n";
       cin.clear();
       cin.ignore(numeric_limits<streamsize>::max(), '\n');
       return selectAction();
   }
   cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
   return c;
}

string CLI::askFilePath(){
    cout<<"Enter File Path: ";
    string path;
    getline(cin,path);
    return path;
}