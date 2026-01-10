#include "pch.h"
#include "Config.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

std::string trim(const std::string &s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start)) ++start;

  auto end = s.end();
  while (end != start && std::isspace(*(end - 1))) --end;

  return std::string(start, end);
}

std::string toLower(const std::string &s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

bool loadConfig(const std::string &filepath, ConfigCommon &config) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "[readIniConfig] INI 파일 열기 실패: " << filepath
              << std::endl;
    return false;
  }

  std::string line;
  std::string currentSection;

  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty() || line[0] == ';' || line[0] == '#') continue;

    // 섹션 구분
    if (line.front() == '[' && line.back() == ']') {
      currentSection = trim(line.substr(1, line.size() - 2));
      continue;
    }

    auto eqPos = line.find('=');
    if (eqPos == std::string::npos) continue;

    std::string key = trim(line.substr(0, eqPos));
    std::string value = trim(line.substr(eqPos + 1));

    if (currentSection == "LC") {
      if (key == "SendIP") {
        config.LCSendIP = value;
      } else if (key == "SendPort") {
        try {
          config.LCSendPort = std::stoi(value);
        } catch (...) {
          std::cerr << "[readIniConfig] LC SendPort 변환 실패: " << value
                    << std::endl;
        }
      }
    } else if (currentSection == "MAP") {
      if (key == "SendIP") {
        config.MAPSendIP = value;
      } else if (key == "SendPort") {
        try {
          config.MAPSendPort = std::stoi(value);
        } catch (...) {
          std::cerr << "[readIniConfig] MAP SendPort 변환 실패: " << value
                    << std::endl;
        }
      }
    }
  }

  file.close();
  return true;
}
