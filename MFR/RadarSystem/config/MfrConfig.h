#ifndef MFR_CONFIG_H
#define MFR_CONFIG_H

#include <string>
#include <termios.h>
#include <cstdint>

struct MfrConfig
{
    std::string launchControllerIP;
    int launchControllerPort = 0;

    int simulatorPort = 0;

    std::string device;
    int uartBaudRate = B9600; // termios 매크로 값으로 초기화
};

// 공백 제거 및 문자열 처리 유틸 함수 선언
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

// ini 파일을 읽어 MfrConfig 구조체를 채움
bool loadMfrConfig(const std::string &filepath, MfrConfig &config);

#endif // MFR_CONFIG_H
