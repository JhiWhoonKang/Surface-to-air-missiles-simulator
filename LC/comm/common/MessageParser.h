//MessageParser.h

#pragma once

#include "CommonMessage.h"
#include "SenderType.h"
#include <vector>
#include <cstdint>

namespace Common {

class MessageParser {
public:
    static CommonMessage parse(const std::vector<uint8_t>& raw, SenderType sender);
};

}
