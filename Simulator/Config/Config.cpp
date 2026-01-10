#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cctype>

std::string trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start))
        start++;

    auto end = s.end();
    do
    {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

std::string toLower(const std::string &s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return result;
}

bool loadConfig(const std::string &filepath, ConfigCommon &config)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "[loadSIMConfig] INI 파일 열기 실패: " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            currentSection = trim(line.substr(1, line.size() - 2));
            continue;
        }

        auto eqPos = line.find('=');
        if (eqPos == std::string::npos)
            continue;

        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));

        if (currentSection == "MFR")
        {
            if (key == "SendIP")
            {
                config.MFRSendIP = value;
            }
            else if (key == "SendPort")
            {
                config.MFRSendPort = std::stoi(value);
            }
        }
        else if (currentSection == "LS")
        {
            if (key == "RecvPort")
            {
                config.LSRecvPort = std::stoi(value);
            }
        }
    }

    file.close();
    return true;
}
