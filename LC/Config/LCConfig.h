#ifndef LC_CONFIG_H
#define LC_CONFIG_H

#include <string>
#include <termios.h>

struct ConfigCommon
{
    std::string ECCRecvIP;
    int ECCRecvPort = 0; // ECC Receive Port

    std::string MFRRecvIP; // MFR Receive IP
    int MFRRecvPort = 0;

    std::string LSSendIP;
    int LSRecvPort = 0;
    int LSSendPort = 0;
};

// 공백 제거 및 문자열 처리 유틸 함수 선언
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

// ini 파일을 읽어 LCConfig 구조체를 채움
bool loadConfig(const std::string &filepath, ConfigCommon &config);

#endif // LC_CONFIG_H