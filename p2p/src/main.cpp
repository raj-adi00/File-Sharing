#include <iostream>
#include "core/Logger.h"
#include "core/ConfigManager.h"

int main() {
    ConfigManager config;
    if (!config.load("config.ini")) {
        std::cout << "Failed to load config\n";
        return 1;
    }

    std::string logFile = config.getString("log_file", "default.log");

    Logger::instance().init(logFile);
    Logger::instance().info("P2P System Starting...");
    Logger::instance().debug("Debug mode enabled");
    Logger::instance().error("Dummy error test");

    int port = config.getInt("port");
    bool debug = config.getBool("debug");

    Logger::instance().info("Port: " + std::to_string(port));
    Logger::instance().info("Debug Mode: " + std::string(debug ? "true" : "false"));

    return 0;
}