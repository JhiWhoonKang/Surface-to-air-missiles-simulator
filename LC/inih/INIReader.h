#pragma once

#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include "ini.h"

class INIReader {
public:
    INIReader(const std::string& filename) {
        _error = ini_parse(filename.c_str(), ValueHandler, this);
    }

    int ParseError() const { return _error; }

    std::string Get(const std::string& section, const std::string& name, const std::string& default_value) const {
        std::string key = MakeKey(section, name);
        return _values.count(key) ? _values.at(key) : default_value;
    }

    int GetInteger(const std::string& section, const std::string& name, int default_value) const {
        std::string val = Get(section, name, "");
        return val.empty() ? default_value : std::stoi(val);
    }

    long long GetLongLong(const std::string& section, const std::string& name, long long default_value) const {
        std::string val = Get(section, name, "");
        return val.empty() ? default_value : std::stoll(val);
    }

    double GetReal(const std::string& section, const std::string& name, double default_value) const {
        std::string val = Get(section, name, "");
        return val.empty() ? default_value : std::stod(val);
    }

    bool HasSection(const std::string& section) const {
        for (const auto& [key, _] : _values) {
            if (key.rfind(section + ".", 0) == 0)
                return true;
        }
        return false;
    }
    bool GetBoolean(const std::string& section, const std::string& name, bool default_value) const {
    std::string val = Get(section, name, "");
    if (val.empty()) return default_value;

    std::string lower;
    for (char c : val) lower += std::tolower(c);

    if (lower == "true" || lower == "yes" || lower == "1") return true;
    if (lower == "false" || lower == "no" || lower == "0") return false;

    return default_value;
    }


private:
    int _error;
    std::map<std::string, std::string> _values;

    static std::string MakeKey(const std::string& section, const std::string& name) {
        return section + "." + name;
    }

    static int ValueHandler(void* user, const char* section, const char* name, const char* value) {
        INIReader* reader = reinterpret_cast<INIReader*>(user);
        std::string key = MakeKey(section, name);
        reader->_values[key] = value;
        return 1;
    }
};
