#include "MfrLcCommManager.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>

#define LC_SERVER_IP "192.168.1.98"
#define LC_SERVER_PORT 9999
#define BUFFER_SIZE 1024

MfrLcCommManager::MfrLcCommManager(IReceiver* receiver) : receiver(receiver), sockfd(-1) 
{
    initMfrLcCommManager();    
}

void MfrLcCommManager::initMfrLcCommManager()
{
    // std::cout << "[Mfr::startTcp] TCP 통신 수신 시작" << std::endl;

    if (loadMfrConfig("../config/MFR.ini", mfrConfig)) 
    {
        // std::cout << "Device: " << mfrConfig.device << ", BaudRate: " << mfrConfig.uartBaudRate << std::endl;

        lcIp = mfrConfig.launchControllerIP;
        lcPort = mfrConfig.launchControllerPort;

        if(connectToLc())
        {
            startTcpReceiver();
        }
    }

    else
    {
        // std::cerr << "[MfrLcCommManager::initMfrLcCommManager] ini Read Failed" << std::endl;
    }
}

bool MfrLcCommManager::connectToLc() 
{
    // std::cout << "[MfrLcCommManager] 서버 연결 시도 중..." << std::endl;

    while (true)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
        {
            // std::cerr << "[MfrLcCommManager::connectToLc] 소켓 생성 실패" << std::endl;
            return false;  // 소켓 자체가 안되면 포기
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(lcPort);
        inet_pton(AF_INET, lcIp.c_str(), &serverAddr.sin_addr);

        if (connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) 
        {
            // std::cout << "[MfrLcCommManager] LC 서버에 연결 성공" << std::endl;
            return true;
        }
        else
        {
            // perror("[MfrLcCommManager] 서버 연결 실패 - 재시도");
            close(sockfd);
            sockfd = -1;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));  // 2초 대기 후 재시도
    }
}

void MfrLcCommManager::startTcpReceiver()
{
    std::thread([this]() {
        char buffer[BUFFER_SIZE];

        // std::cout << "[MfrLcCommManager::startTcpReceiver] TcpReceiver 스레드 시작" << std::endl;

        while (true)
        {
            ssize_t len = read(this->sockfd, buffer, sizeof(buffer));
            if (len <= 0)
            {
                // std::cerr << "[MfrLcCommManager::startTcpReceiver] 연결 종료 또는 수신 실패" << std::endl;
                break;
            }

            std::vector<char> packet(buffer, buffer + len);
            // std::cout << "[MfrLcCommManager::startTcpReceiver] 수신된 패킷: " << len << " bytes" << std::endl;

            if (this->receiver)
            {
                this->receiver->callBackData(packet);
            }
            else
            {
                // std::cerr << "[MfrLcCommManager::startTcpReceiver] receiver가 null임" << std::endl;
            }
        }
    }).detach();
}

void MfrLcCommManager::send(const std::vector<char>& packet) 
{
    if (sockfd < 0) 
    {
        // std::cerr << "[MfrLcCommManager::send] 소켓이 미 Open" << std::endl;
        return;
    }

    ssize_t sent = write(sockfd, packet.data(), packet.size());
    // std::cout << "[MfrLcCommManager::send] 전송 시도, size: " << packet.size() << " bytes" << std::endl;
    if (sent != packet.size()) 
    {
        // std::cerr << "[MfrLcCommManager::send] 전송 실패" << std::endl;
    }

    else 
    {
        // std::cout << "[MfrLcCommManager::send] send() 성공, size: " << sent << " bytes" << std::endl;
    }
}
