#include "Logger.h"
#include <iostream>
#include <ctime>
using namespace std;

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const string& filePath) {
    lock_guard<mutex> lock(mtx);
    file.open(filePath, ios::app);
}

Logger::~Logger() {
    if (file.is_open()) file.close();
}

string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::LOG_INFO: return "INFO";
        case LogLevel::LOG_DEBUG: return "DEBUG";
        case LogLevel::LOG_ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::log(LogLevel level, const string& message, bool logToTerminal) {
    lock_guard<mutex> lock(mtx);

    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string finalMsg = "[" + string(buf) + "][" + levelToString(level) + "] " + message;
    
    if(logToTerminal)cout << finalMsg << endl;
    if (file.is_open()) file << finalMsg << endl;
}

void Logger::info(const string& msg) {
    log(LogLevel::LOG_INFO, msg);
}

void Logger::debug(const string& msg) {
    log(LogLevel::LOG_DEBUG, msg);
}

void Logger::error(const string& msg) {
    log(LogLevel::LOG_ERROR, msg);
}

void Logger::logToFile(const string &msg){
    log(LogLevel::LOG_FILE, msg, false);
}