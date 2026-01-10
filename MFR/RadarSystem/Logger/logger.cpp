#include "logger.h"
#include <iostream>

std::mutex Logger::mtx; // 정적 멤버 변수 정의

void Logger::log(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << message << std::endl;
}