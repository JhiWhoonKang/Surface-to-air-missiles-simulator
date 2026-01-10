#pragma once
#include "MessageCommon.h"

#pragma pack(push, 1)

struct RadarModeChangeAck {
    CommandType commandType = CommandType::RADAR_MODE_CHANGE;
    uint8_t radarId;
    uint8_t newMode;  // 서버가 반영한 모드
};

#pragma pack(pop)
