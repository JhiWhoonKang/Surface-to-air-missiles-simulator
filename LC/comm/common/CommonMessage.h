#pragma once

#include "CommandType.h"
#include "SenderType.h"
#include <vector>
#include <cstdint>
#include <variant>
#include <string>

namespace Common {

// 내부 열거형: 발사대 동작 모드
enum class OperationMode : uint8_t {
    WAR_MODE = 0,
    MOVE_MODE = 1,
    STOP_MODE = 2
};


// [0x13] LC → MFR: 위치 응답
struct LCPositionResponse {
    unsigned int radarId;
    long long posX;
    long long posY;
    long long height;
};

// [0x02] ECC → LC: 레이더 모드 변경
struct RadarModeCommand {
    unsigned int radarId;
    uint8_t radarMode;     // 예: 1 = STOP, 2 = ROTATE 등
    uint8_t flag;          // 조건에 따른 모드 보조 정보
    uint8_t priority_select;
    unsigned int targetId; // 수동 지정일 때만 유효
};

// [0x03] ECC → LC: 발사대 모드 설정
struct LauncherCommand {
    unsigned int lsId;
    uint8_t lsMode;
};

// [0x04] ECC → LC: 유도탄 발사 (→ LaunchCommand로 전달됨)
struct FireCommand {
    unsigned int lsId;
    unsigned int targetId;
};

// [0x05] ECC → LC: 발사대 이동 명령 (→ MoveCommand로 전달됨)
struct MoveCommand {
    unsigned int lsId;
    long long posX;
    long long posY;
};

// [0x21] MFR → LC: 상태 보고
struct RadarStatus {
    unsigned int radarId;
    long long posX;
    long long posY;
    long long height;
    uint8_t radarMode;
    double radarAngle;
    unsigned int mockTargetId;
    unsigned int mockMissileId;

};

// [0x22] MFR → LC: 탐지 정보 보고
struct RadarDetection {
    struct Target {
        unsigned int id;
        long long posX;
        long long posY;
        long long altitude;
        int speed;
        double angle1;
        double angle2; // 고도각
        unsigned long long detectTime;
        uint8_t priority;
        bool hit;
    };
    struct Missile
    {
        unsigned int id; 
        long long posX;
        long long posY;
        long long altitude;
        int speed;
        double angle;
        unsigned long long detectTime;
        unsigned long long interceptTime;
        bool hit;
    };

    unsigned int radarId;
    std::vector<Target> targets;
    std::vector<Missile> missiles;

    // [미사용] 향후 메시지 로그 전송 용도 (256 bytes)
    // std::string message;

    // [미사용] 탐지 결과 상태 요약용 플래그
    // uint8_t resultFlag = 0;
};

// [0x31] LC → LS: 유도탄 발사 명령
struct LaunchCommand {
    unsigned int launcherId;
    double launchAngleXY;  
    double launchAngleXZ;
    long long start_x;
    long long start_y;
    long long start_z;
};

// [0x32] LC → LS: 이동 명령
struct MoveCommandLS {
    unsigned int launcherId;
    long long newX;
    long long newY;
};

// [0x33] LC → LS: 모드 변경
struct LauncherModeCommand {
    unsigned int launcherId;
    OperationMode newMode;  // enum: WAR, MOVE, STOP
};

struct LSReport {
    unsigned int lsId;       // 4 bytes
    long long posX;          // 8 bytes
    long long posY        ;            // 8 bytes
    long long height;        // 8 bytes
    int speed;               // 4 bytes
    double launchAngle;
    OperationMode mode;      // 1 byte
};


// 공통 메시지 포맷
struct CommonMessage {
    SenderType sender;
    CommandType commandType;
    bool ok = true;

    std::variant<
    std::monostate,
        RadarModeCommand,    
        LauncherCommand,    
        LauncherModeCommand,     
        FireCommand,
        MoveCommand,
        RadarStatus,
        RadarDetection,
        LaunchCommand,
        MoveCommandLS,
        LCPositionResponse,
        LSReport              
    > payload;
};

} // namespace Common
