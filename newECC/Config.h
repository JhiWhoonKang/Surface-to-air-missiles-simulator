#ifndef ECC_CONFIG_H
#define ECC_CONFIG_H

#include <string>

struct ConfigCommon {
  std::string LCSendIP;
  int LCSendPort = 0;  // Launch Controller IP and Port

  std::string MAPSendIP;
  int MAPSendPort = 0;  // Map Server IP and Port
};

// 공백 제거 및 문자열 처리 유틸 함수 선언
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

// ini 파일을 읽어 MfrConfig 구조체를 채움
bool loadConfig(const std::string &filepath, ConfigCommon &config);

#endif  // ECC_CONFIG_H
