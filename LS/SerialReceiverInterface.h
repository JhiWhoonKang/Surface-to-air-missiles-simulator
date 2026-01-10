#pragma once

#include <vector>
#include <cstdint>

class SerialReceiverInterface {
public:
    virtual ~SerialReceiverInterface() = default;

    // callBack 가상함수
    virtual void callBack(const std::vector<uint8_t>& data) = 0;
};