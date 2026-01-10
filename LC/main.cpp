#include "main.h"
#include <iostream>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>

int main() {
    LCManager manager;
    manager.init("./config/system_config.ini", "0.0.0.0", 8888);  // ✅ 먼저 초기화
    manager.run();                                              // ✅ 이후 디버그 루프 백그라운드 실행

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}