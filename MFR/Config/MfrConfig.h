#ifndef MFR_CONFIG_H
#define MFR_CONFIG_H

#include <string>
#include <termios.h>

class MfrConfig
{
private:
    MfrConfig() = default; // private 생성자
    static MfrConfig *instance;

public:
    // 싱글톤 인스턴스 접근자
    static MfrConfig &getInstance()
    {
        if (instance == nullptr)
        {
            instance = new MfrConfig();
        }
        return *instance;
    }

    // 복사 및 할당 방지
    MfrConfig(const MfrConfig &) = delete;
    MfrConfig &operator=(const MfrConfig &) = delete;

    // 설정 멤버 변수들
    std::string launchControllerIP;
    int launchControllerPort = 0;
    int simulatorPort = 0;
    std::string device;
    int uartBaudRate = B9600;

    // 모터 제어 서버
    std::string motorControllerIp;
    int motorControllerPort = 0;

    // 설정 로드 메서드
    bool loadConfig(const std::string &filepath);
};

// 공백 제거 및 문자열 처리 유틸 함수 선언
std::string trim(const std::string &s);
std::string toLower(const std::string &s);

#endif // MFR_CONFIG_H