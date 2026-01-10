#include "MfrConfig.h"
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

bool loadMfrConfig(const std::string &filepath, MfrConfig &config)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "[loadMfrConfig] INI 파일 열기 실패: " << filepath << std::endl;
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

        if (currentSection == "LaunchController")
        {
            if (key == "IP")
            {
                config.launchControllerIP = value;
            }
            else if (key == "Port")
            {
                config.launchControllerPort = std::stoi(value);
            }
        }
        else if (currentSection == "Simulator")
        {
            if (key == "Port")
            {
                config.simulatorPort = std::stoi(value);
            }
        }
        else if (currentSection == "Motor")
        {
            if (key == "Device")
            {
                config.device = value;
            }
            else if (key == "BaudRate")
            {
                int baudInt = std::stoi(value);
                switch (baudInt)
                {
                case 9600:
                    config.uartBaudRate = B9600;
                    break;
                case 19200:
                    config.uartBaudRate = B19200;
                    break;
                case 38400:
                    config.uartBaudRate = B38400;
                    break;
                case 57600:
                    config.uartBaudRate = B57600;
                    break;
                case 115200:
                    config.uartBaudRate = B115200;
                    break;
                default:
                    std::cerr << "[loadMfrConfig] 지원하지 않는 BaudRate: " << baudInt << std::endl;
                    config.uartBaudRate = B9600;
                }
            }
        }
    }
    file.close();
    return true;
}
