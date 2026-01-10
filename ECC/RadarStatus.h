#pragma once

#include "MessageCommon.h"  // Pos2D

#pragma pack(push, 1)

struct RadarStatus {
    enum Mode : unsigned int {
        STOP = 1,
        ROTATE = 2
    };
    unsigned int id;
    Pos2D position;
    uint8_t mode;
    double angle;
};

#pragma pack(pop)
