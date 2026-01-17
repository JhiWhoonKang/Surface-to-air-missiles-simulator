#pragma once
#include <mutex>
#include <string>

class Logger
{
private:
    static std::mutex mtx;

public:
    static void log(const std::string &message);
};