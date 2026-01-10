#pragma once
#include <cstddef>
#include "ParsedPacket.h"

class PacketParser {
public:
    static ParsedPacket Parse(const char* buffer, size_t length);
};
