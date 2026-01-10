#pragma once
#include <cstdint>

// 1. Command Type 정의 (송수신 구분 없이 0x01~0x05 공통)
enum class CommandType : uint8_t {
    STATUS_REQUEST = 0x01,
    RADAR_MODE_CHANGE = 0x02,
    LS_MODE_CHANGE = 0x03,
    MISSILE_LAUNCH = 0x04,
	LS_MOVE = 0x05,
	STATUS_RESPONSE = 0x51, // 상태 응답 메시지
	RADAR_MODE_CHANGE_ACK = 0x52, // 레이더 모드 변경 응답
	LS_MODE_CHANGE_ACK = 0x53, // 발사대 모드 변경 응답
	MISSILE_LAUNCH_ACK = 0x54, // 유도탄 발사 응답
	LS_MOVE_ACK = 0x55 // 발사대 이동 응답
};

struct Pos2D {
    long long x;
    long long y;
	long long z=0;
};
