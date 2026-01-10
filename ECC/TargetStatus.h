#pragma once

#include "MessageCommon.h"  // Pos2D

#pragma pack(push, 1)

struct TargetStatus { // 50
    unsigned int id; // 4
    Pos2D position; // 24
    //long long height;
    int speed; // 4
    double angle1; // 8
	double angle2; // 8
    unsigned long long first_detect_time; // 8
    uint8_t priority; // 1
    bool hit; // 1
};

#pragma pack(pop)
