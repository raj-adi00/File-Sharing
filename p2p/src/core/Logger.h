#pragma once
#include <string>
#include <fstream> //used for writing to the log file
#include <mutex> //used for locking the file when multiple threads are writing to it
using namespace std;

enum class LogLevel {
    INFO,
    DEBUG,
    ERROR
};

class Logger {
public:
    static Logger& instance();

    void init(const string& filePath);
    void log(LogLevel level, const string& message);

    void info(const string& msg);
    void debug(const string& msg);
    void error(const string& msg);

private:
    Logger() = default;
    ~Logger();

    ofstream file;
    mutex mtx;

    string levelToString(LogLevel level);
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
