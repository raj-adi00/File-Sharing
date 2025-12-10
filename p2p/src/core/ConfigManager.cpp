#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

bool ConfigManager::load(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find('=');
        if (pos == string::npos) continue;

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());

        data[key] = value;
    }
    return true;
}

string ConfigManager::getString(const string& key, const string& def) {
    return data.count(key) ? data[key] : def;
}

int ConfigManager::getInt(const string& key, int def) {
    return data.count(key) ? stoi(data[key]) : def;
}

bool ConfigManager::getBool(const string& key, bool def) {
    if (!data.count(key)) return def;
    return data[key] == "true" || data[key] == "1";
}
