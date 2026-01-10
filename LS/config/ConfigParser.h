#pragma once
#include <string>

class ConfigParser {
public:
    // section과 key로부터 문자열 값 반환
    static std::string getValue(const std::string& section, const std::string& key, const std::string& path);

    // string → int, long long, double 변환 유틸
    static int getInt(const std::string& section, const std::string& key, const std::string& path);
    static long long getLongLong(const std::string& section, const std::string& key, const std::string& path);
    static double getDouble(const std::string& section, const std::string& key, const std::string& path);
};
