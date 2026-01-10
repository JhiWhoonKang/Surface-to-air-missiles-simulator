#pragma once
#include <vector>
#include <cstdint>

class LCToLSCommInterface 
{
public:
    virtual ~LCToLSCommInterface() = default;
    virtual void sendData(const std::vector<uint8_t>& packet) = 0;
};
