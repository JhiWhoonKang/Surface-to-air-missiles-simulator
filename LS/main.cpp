#include <iostream>
#include "LS.h"
#include <string>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "[Usage] " << argv[0] << " <main_config_path>\n";
        return 1;
    }

    try
    {
        // 초기 config 파일 경로 입력 받아 해당 파일로 LS 객체 생성
        std::string path = argv[1];
        LS ls(path);  

        std::cout << "[Main] Launcher System running.\n";
        std::cout << "Press Ctrl+C to terminate.\n";

        // 프로그램 계속 유지
        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) 
    {
        std::cerr << "[Error] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
