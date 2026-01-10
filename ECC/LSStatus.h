#pragma once

#include "MessageCommon.h"  // Pos2D

#pragma pack(push, 1)
struct LSStatus {
    enum Mode : unsigned int {
        WAR = 0,
        MOVE = 1,
        STOP = 2
    };
    unsigned int id;
    Pos2D position;
    uint8_t mode;
    double angle;
};

#pragma pack(pop)
