// TransmitStructs.h
#pragma once
#include "MessageCommon.h"

#pragma pack(push, 1)

struct RadarStatus {
    uint8_t id;
    Pos2D position;
    uint8_t mode;    // 0 = STOP, 1 = ROTATE
    double angle;
};

struct LCStatus {
    uint8_t id;
    Pos2D position;
};

struct LSStatus {
    uint8_t id;
    uint8_t mode;  // 0 = STOP, 1 = MOVE, 2 = WAR
    Pos2D position;
    double angle;
};

struct MissileStatus {
    uint8_t id;
    Pos2D position;
    double degree;
    uint8_t locked;
};

struct TargetStatus {
    uint8_t id;
    Pos2D position;
    double degree;
    double speed;
};

struct StatusHeader {
    CommandType commandType = CommandType::STATUS_REQUEST;
    uint8_t num_radar;
    uint8_t num_lc;
    uint8_t num_ls;
    uint8_t num_target;
    uint8_t num_missile;
};

RadarStatus ConvertToRadarStatus(const RadarStatus& mfr);
LCStatus ConvertToLCStatus(const ::LCStatus& lc);
LSStatus ConvertToLSStatus(const ::LSStatus& ls);
MissileStatus ConvertToMissileStatus(const ::MissileStatus& m);
TargetStatus ConvertToTargetStatus(const ::TargetStatus& t);


#pragma pack(pop)
