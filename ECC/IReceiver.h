#pragma once

class IReceiver {
public:
    virtual ~IReceiver() = default;

    // 수신된 raw 데이터 전달
    virtual void receive(int len, const char* packet) = 0;
};
