#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "generic_tools.h"

std::unordered_map<std::string, std::string> parse_config_file(const std::string& filename) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return config;
    }
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Find the position of the equal sign
        std::size_t pos = line.find('=');
        if (pos != std::string::npos) {
            // Extract key and value from the line
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // Trim leading and trailing whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            // Store key-value pair in the configuration map
            config[key] = value;
        }
    }
    file.close();
    return config;

}