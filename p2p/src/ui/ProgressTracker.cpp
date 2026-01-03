#include "ProgressTracker.h"
#include<iostream>
#include<iomanip>

using namespace std;

ProgressTracker::ProgressTracker(size_t totalBytes,size_t initialOffset):total(totalBytes),transferred(0),offset(initialOffset),start(chrono::steady_clock::now()){}

void ProgressTracker::update(size_t bytes){
    transferred+=bytes;
}

void ProgressTracker::render(){
    using namespace std::chrono;

    double progress=(double)(offset+transferred)/total;
    auto now=steady_clock::now();
    double elapsed=duration_cast<duration<double>>(now-start).count();
    double speed=elapsed>0?transferred/elapsed:0;
    int barWidth=20;
    int filled=(int)(progress*barWidth);
    cout<<"\r[";
    for(int i=0;i<barWidth;i++){
        cout<<(i<filled?"#":"-");
    }
    cout<<"] ";
    cout<<setw(3)<<setfill(' ')<<(int)(progress*100)<<"% | ";
    cout<<fixed<<setprecision(2)<<(speed/(1024*1024))<<" MB/s | ";
    if(progress>=1)
        cout<<"ETA 0:00 | ";
    else{
        double eta=speed>0?(total-transferred)/speed:0;
        int mins=(int)eta/60;
        int secs=(int)eta%60;
        cout<<"ETA "<<setw(2)<<setfill('0')<<mins<<":"<<setw(2)<<setfill('0')<<secs<<" | ";
    }
    cout.flush();
}