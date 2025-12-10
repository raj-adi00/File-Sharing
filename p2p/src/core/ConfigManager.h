#pragma once
#include <string>
#include <unordered_map>
using namespace std;

class ConfigManager {
public:
    bool load(const string& filePath);

    string getString(const string& key, const string& def = "");
    int getInt(const string& key, int def = 0);
    bool getBool(const string& key, bool def = false);

private:
    unordered_map<string, string> data;
};
