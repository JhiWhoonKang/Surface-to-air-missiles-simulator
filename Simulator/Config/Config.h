#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <string>
#include <termios.h>

struct ConfigCommon
{
    int LSRecvPort = 0;    // Launch Simulator Port
    std::string MFRSendIP; // Launch Controller IP
    int MFRSendPort = 0;   // Launch Controller Port
};

// 공백 제거 및 문자열 처리 유틸 함수 선언
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

// ini 파일을 읽어 SIMConfig 구조체를 채움
bool loadConfig(const std::string &filepath, ConfigCommon &config);

#endif // SIM_CONFIG_H
