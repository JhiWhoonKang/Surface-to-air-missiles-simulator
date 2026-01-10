#pragma once
#include "SystemStatus.h"
#include "CommonMessage.h"
#include <vector>
#include <string>

namespace Common {

class Serializer {
public:
    // 공통 응답/명령 직렬화
    static std::vector<uint8_t> serializeStatusResponse(const SystemStatus& status);
    // static std::vector<uint8_t> serializeRadarResponse(unsigned radarId, uint8_t radarMode, bool ok, const std::string& msg);
    static std::vector<uint8_t> serializeLauncherResponse(unsigned lsId, uint8_t lsMode, bool ok, const std::string& msg);
    static std::vector<uint8_t> serializeConsoleResponse(uint8_t consoleId, uint8_t mode, bool ok, const std::string& msg);
    static std::vector<uint8_t> serializeMessage(const CommonMessage& msg);

    // 레이더 제어 관련
    static std::vector<uint8_t> serializeRadarStatusRequest(unsigned int radarId);
    static std::vector<uint8_t> serializeRadarModeChange(const RadarModeCommand& cmd);

    // LC 위치 응답
    static std::vector<uint8_t> serializeLCPositionResponse(const LCPositionResponse& res);

    // 발사 관련
    static std::vector<uint8_t> serializeLaunchCommand(const LaunchCommand& cmd);
    static std::vector<uint8_t> serializeFireCommandToLS(const FireCommand& cmd);

    // 이동 및 모드 변경 관련
    static std::vector<uint8_t> serializeMoveCommandLS(const MoveCommandLS& cmd);
    static std::vector<uint8_t> serializeModeChangeCommand(const LauncherModeCommand& cmd);
    static std::vector<uint8_t> serializeLauncherModeCommand(const LauncherCommand& cmd);
    static std::vector<uint8_t> serializeLauncherCommand(const LauncherCommand& cmd);   
};

}
