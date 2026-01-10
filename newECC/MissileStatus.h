#pragma once

#include "MessageCommon.h"  // Pos2D

#pragma pack(push, 1)

struct MissileStatus { // 57
    unsigned int id; // 4
    Pos2D position; // 24
    //long long height;
    int speed; // 4
    double angle; // 8
    unsigned long long predicted_time; // 8
    unsigned long long intercept_time; // 8
    bool hit; // 1
};

#pragma pack(pop)
