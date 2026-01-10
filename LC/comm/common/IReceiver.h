
//IRece
#pragma once

#include "CommonMessage.h"
#include "IReceiverCallback.h"  
#include "SenderType.h"
#include <vector>
#include <cstdint>  // uint8_t 안 쓰고 있으면 이건 생략 가능

class IReceiver {
public:
    virtual ~IReceiver() = default;

    virtual void handleReceived(const std::vector<uint8_t>& data, SenderType from) = 0;

    virtual void start() = 0;
    virtual SenderType getSenderType() const = 0;  // 여전히 자기 타입을 위한 용도
    virtual void setCallback(IReceiverCallback* cb) = 0;
};
