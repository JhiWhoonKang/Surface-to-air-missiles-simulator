#include "Mfr.hpp"
#include <thread>
#include <iostream>
#include "logger.h"

using namespace std;

int main()
{
    Mfr mfr;

    while (true)
    {
        // std::cout << "MFR 시스템이 실행 중입니다..." << std::endl;
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    return 0;
}