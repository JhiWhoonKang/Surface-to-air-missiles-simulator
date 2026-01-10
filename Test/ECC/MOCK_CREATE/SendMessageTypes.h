#pragma once
#include <cstdint>

#pragma pack(push, 1)

enum class CommandType : uint8_t {
  STATUS_REQUEST = 0x01,
  RADAR_MODE_CHANGE = 0x02,
  LS_MODE_CHANGE = 0x03,
  MISSILE_LAUNCH = 0x04,
  LS_MOVE = 0x05,
  STATUS_RESPONSE = 0x51,        // 상태 응답 메시지
  RADAR_MODE_CHANGE_ACK = 0x52,  // 레이더 모드 변경 응답
  LS_MODE_CHANGE_ACK = 0x53,     // 발사대 모드 변경 응답
  MISSILE_LAUNCH_ACK = 0x54,     // 유도탄 발사 응답
  LS_MOVE_ACK = 0x55             // 발사대 이동 응답
};

struct Pos2D {
  long long x;
  long long y;
};

struct StatusHeader {
  CommandType commandType = CommandType::STATUS_RESPONSE;
  uint8_t num_radar;
  uint8_t num_lc;
  uint8_t num_ls;
  uint8_t num_target;
  uint8_t num_missile;
};

struct RadarStatus {
  enum Mode : unsigned int { STOP = 0, ROTATE = 1 };
  unsigned int id;
  Pos2D position;
  uint8_t mode;
  double angle;
};

struct LSStatus {
  enum Mode : unsigned int { STOP = 0, MOVE = 1, WAR = 2 };
  unsigned int id;
  Pos2D position;
  uint8_t mode;
  double angle;
};

struct LCStatus {
  unsigned int id;
  Pos2D position;
};

struct MissileStatus {
  unsigned int id;
  Pos2D position;
  int64_t height;
  int speed;
  double angle;
  unsigned long long predicted_time;
  unsigned long long intercept_time;
  bool hit;
};

struct TargetStatus {
  unsigned int id;
  Pos2D position;
  int64_t height;
  int speed;
  double angle;
  unsigned long long first_detect_time;
  uint8_t priority;
  bool hit;
};

#pragma pack(pop)