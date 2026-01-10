#include "ConfigParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t");
    auto end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::string ConfigParser::getValue(const std::string& section, const std::string& key, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Unable to open file: " + path);

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) 
    {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        if (line.front() == '[' && line.back() == ']')
        {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        if (currentSection == section)
        {
            auto delimPos = line.find('=');
            if (delimPos == std::string::npos) continue;

            std::string k = trim(line.substr(0, delimPos));
            std::string v = trim(line.substr(delimPos + 1));
            if (k == key) return v;
        }
    }

    throw std::runtime_error("Key '" + key + "' not found in section [" + section + "] of file: " + path);
}

int ConfigParser::getInt(const std::string& section, const std::string& key, const std::string& path)
{
    return std::stoi(getValue(section, key, path));
}

long long ConfigParser::getLongLong(const std::string& section, const std::string& key, const std::string& path)
{
    return std::stoll(getValue(section, key, path));
}

double ConfigParser::getDouble(const std::string& section, const std::string& key, const std::string& path)
{
    return std::stod(getValue(section, key, path));
}