//SenderType.h

#pragma once
#include "CommandType.h"    
#include <cstdint>

enum class SenderType : uint8_t {
    ECC     = 0x01,
    MFR     = 0x02,
    LS      = 0x03,
    Unknown = 0xFF
};