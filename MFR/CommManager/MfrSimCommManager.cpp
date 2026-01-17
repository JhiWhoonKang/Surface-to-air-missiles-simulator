#include "MfrSimCommManager.h"
#include "PacketProtocol.h"
#include "logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <iostream>

namespace
{
    constexpr size_t BUFFER_SIZE = 1024;
}

MfrSimCommManager::MfrSimCommManager(std::shared_ptr<IReceiver> receiver)
    : receiver_(std::move(receiver)), sockfd(-1), simPort(0), isRunning_(false)
{
    initMfrSimCommManager();
}

MfrSimCommManager::~MfrSimCommManager()
{
    stopReceiver();
    if (sockfd >= 0)
    {
        close(sockfd);
        Logger::log("[MfrSimCommManager] Socket closed");
    }
}

void MfrSimCommManager::initMfrSimCommManager()
{
    Logger::log("[MfrSimCommManager] Initializing UDP communication");

    const auto &config = MfrConfig::getInstance();
    simPort = config.simulatorPort;

    Logger::log("[MfrSimCommManager] Initializing with Simulator Port: " + std::to_string(simPort));

    if (connectToSim())
    {
        startUdpReceiver();
    }
}

bool MfrSimCommManager::connectToSim()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        Logger::log("[MfrSimCommManager] Failed to create UDP socket");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(simPort);

    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        Logger::log("[MfrSimCommManager] Failed to bind UDP socket");
        close(sockfd);
        return false;
    }

    Logger::log("[MfrSimCommManager] Successfully connected to simulator");
    return true;
}

void MfrSimCommManager::startUdpReceiver()
{
    if (isRunning_)
    {
        Logger::log("[MfrSimCommManager] Receiver thread already running");
        return;
    }

    isRunning_ = true;
    receiverThread = std::thread(&MfrSimCommManager::runReceiver, this);
}

void MfrSimCommManager::runReceiver()
{
    Logger::log("[MfrSimCommManager] UDP Receiver thread started");

    std::vector<char> buffer(BUFFER_SIZE);
    sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    while (isRunning_)
    {
        // 소켓 타임아웃 설정
        struct timeval tv;
        tv.tv_sec = 1; // 1초 타임아웃
        tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        ssize_t len = recvfrom(sockfd, buffer.data(), buffer.size(), 0,
                               reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);

        if (len <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 타임아웃 - 계속 진행
                continue;
            }
            if (isRunning_)
            {
                Logger::log("[MfrSimCommManager] Error receiving data");
            }
            break;
        }

        processReceivedData(buffer.data(), len);
    }

    Logger::log("[MfrSimCommManager] UDP Receiver thread stopped");
}

void MfrSimCommManager::processReceivedData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock(); // weak_ptr를 통한 안전한 접근
    if (!receiver)
    {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    if (len == sizeof(uint8_t) + sizeof(TargetSimData))
    {
        processTargetData(buffer, len);
    }
    else if (len == sizeof(uint8_t) + sizeof(MissileSimData))
    {
        processMissileData(buffer, len);
    }
    else
    {
        Logger::log("[MfrSimCommManager] Invalid packet size: " + std::to_string(len) + " bytes");
    }
}

void MfrSimCommManager::processMissileData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock();
    if (!receiver)
    {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    try
    {
        MissileSimData data;
        std::memcpy(&data, buffer + 1, sizeof(MissileSimData));
        std::vector<char> packet(buffer, buffer + len);
        {
            std::lock_guard<std::mutex> lock(callbackMutex_); // 스레드 안전을 위한 뮤텍스 사용
            receiver->callBackData(packet);
        }
    }
    catch (const std::exception &e)
    {
        Logger::log("[MfrSimCommManager] Error in processMissileData: " + std::string(e.what()));
    }
}

void MfrSimCommManager::stopReceiver()
{
    isRunning_ = false;
    if (receiverThread.joinable())
    {
        receiverThread.join();
    }
    Logger::log("[MfrSimCommManager] Receiver stopped");
}

void MfrSimCommManager::processTargetData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock();
    if (!receiver)
    {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    try
    {
        TargetSimData data;
        std::memcpy(&data, buffer + 1, sizeof(TargetSimData));
        std::vector<char> packet(buffer, buffer + len);
        {
            std::lock_guard<std::mutex> lock(callbackMutex_);
            receiver->callBackData(packet);
        }
    }
    catch (const std::exception &e)
    {
        Logger::log("[MfrSimCommManager] Error in processTargetData: " + std::string(e.what()));
    }
}