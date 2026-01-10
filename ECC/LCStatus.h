#pragma once

#include "MessageCommon.h"  // Pos2D

#pragma pack(push, 1)

struct LCStatus {
    unsigned int id;
    Pos2D position;
};

#pragma pack(pop)
