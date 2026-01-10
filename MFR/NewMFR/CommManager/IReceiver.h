#pragma once
#include <vector>
#include <mutex>

class IReceiver
{
public:
    virtual void callBackData(const std::vector<char> &packet) = 0;
    virtual ~IReceiver() = default;
};