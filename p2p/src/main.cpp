#include <iostream>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/PeerCore.h"
using namespace std;

int main() {
    ConfigManager config;
    if (!config.load("config.ini")) {
        cout << "Failed to load config\n";
        return 1;
    }

    string logFile = config.getString("log_file", "default.log");

    Logger::instance().init(logFile);
    Logger::instance().info("P2P System Starting...");
    Logger::instance().debug("Debug mode enabled");
    Logger::instance().error("Dummy error test");

    int port = config.getInt("port");
    bool debug = config.getBool("debug");

    Logger::instance().info("Port: " + to_string(port));
    Logger::instance().info("Debug Mode: " + string(debug ? "true" : "false"));

    PeerCore core;
    core.start();

    this_thread::sleep_for(chrono::seconds(5));

    core.stop();
    Logger::instance().info("P2P System Shutdown...");

    return 0;
}