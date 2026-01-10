#pragma once
#include <cstdint>

namespace Common {

enum class CommandType : uint8_t {
    // ECC → LC
    STATUS_REQUEST_ECC_TO_LC      = 0x01,
    SET_RADAR_MODE_ECC_TO_LC      = 0x02,
    SET_LAUNCHER_MODE_ECC_TO_LC   = 0x03,
    FIRE_COMMAND_ECC_TO_LC        = 0x04,
    MOVE_COMMAND_ECC_TO_LC        = 0x05,

    // LC → MFR
    STATUS_REQUEST_LC_TO_MFR      = 0x11,
    RADAR_MODE_CHANGE_LC_TO_MFR   = 0x12,
    POSITION_RESPONSE_LC_TO_MFR   = 0x13,
    
    // MFR → LC
    STATUS_RESPONSE_MFR_TO_LC     = 0x21,
    DETECTION_MFR_TO_LC           = 0x22,
    POSITION_REQUEST_MFR_TO_LC    = 0x23,

    //LC -> LS
    LAUNCH_COMMAND_LC_TO_LS       = 0x31,
    MOVE_COMMAND_LC_TO_LS         = 0x32,
    MODE_CHANGE_LC_TO_LS          = 0x33,
    STATUS_REQUEST_LC_TO_LS       = 0x34,
    
    LS_STATUS_UPDATE_LS_TO_LC       = 0x41,  // LC → LS: 발사대 상태 갱신 전송
    // LS_STATUS_RESPONSE_LS_TO_LC     = 0x42 // LC → LS: 발사대 상태 응답(이동 완료 등)

    //LC-> ECC
    STATUS_RESPONSE_LC_TO_ECC         = 0x51 // 상태 전송 (LC 전체 상태)
    // RADAR_RESPONSE_LC_TO_ECC          = 0x52,  // 레이더 모드 변경 응답 등
    // LAUNCHER_RESPONSE_LC_TO_ECC       = 0x53,  // 발사대 모드 변경 응답 등
    // FIRE_RESPONSE_LC_TO_ECC           = 0x54,  // 유도탄 발사 명령 응답
    // MOVE_RESPONSE_LC_TO_ECC           = 0x55   // 이동 명령 응답

};

}
