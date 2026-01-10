#pragma once
#include "MessageCommon.h"

#pragma pack(push, 1)

// [0x01] 상태 요청 ? 1 byte
struct StatusRequest {
    CommandType commandType = CommandType::STATUS_REQUEST;
};

// [0x02] 레이더 모드 변경 ? 4 byte
struct RadarModeChange {
    CommandType commandType = CommandType::RADAR_MODE_CHANGE;
    unsigned int radar_id;
    uint8_t radar_mode;
	uint8_t priority_select; // 1= 최우선 우선순위, 2= 사용자 지정 우선순위
    unsigned int target_id;
};

// [0x03] 발사대 모드 변경 ? 19 byte
struct LSModeChange {
    CommandType commandType = CommandType::LS_MODE_CHANGE;
    unsigned int ls_id;
    uint8_t ls_mode;
};

// [0x04] 유도탄 발사 ? 3 byte
struct MissileLaunch {
    CommandType commandType = CommandType::MISSILE_LAUNCH;
    unsigned int ls_id;
    unsigned int target_id;
};
// [0x05] 발사대 이동 ? 18 byte
struct LSMove {
	CommandType commandType = CommandType::LS_MOVE;
	unsigned int ls_id;
	Pos2D position;  // 16 byte (x + y)
};

#pragma pack(pop)
