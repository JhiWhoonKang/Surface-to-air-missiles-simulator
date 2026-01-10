#pragma once
#include <cstdint>

// 🧭 2D 좌표 정보
struct Pos2D {
    long long x = 0;
    long long y = 0;
};

// 📡 공통 명령 타입 (송신/수신 구분 없이 사용)
enum class CommandType : uint8_t {
    STATUS_REQUEST     = 0x01,  // 상태 요청
    RADAR_MODE_CHANGE  = 0x02,  // 레이더 모드 변경
    LS_MODE_CHANGE     = 0x03,  // 발사대 모드/위치 변경
    MISSILE_LAUNCH     = 0x04   // 유도탄 발사
};
