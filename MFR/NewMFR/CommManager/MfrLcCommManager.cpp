#include "MfrLcCommManager.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>

namespace
{
    constexpr size_t BUFFER_SIZE = 1024;
}

MfrLcCommManager::MfrLcCommManager(std::shared_ptr<IReceiver> receiver)
    : receiver_(receiver), sockfd(-1)
{
    initMfrLcCommManager();
}

MfrLcCommManager::~MfrLcCommManager()
{
    stopReceiver();
    if (sockfd >= 0)
    {
        close(sockfd);
        Logger::log("[MfrLcCommManager] Socket closed");
    }
}

void MfrLcCommManager::initMfrLcCommManager()
{
    const auto &config = MfrConfig::getInstance();
    lcIp = config.launchControllerIP;
    lcPort = config.launchControllerPort;
    Logger::log("[MfrLcCommManager] Initialized with IP: " + lcIp + ", Port: " + std::to_string(lcPort));

    if (connectToLc())
    {
        startTcpReceiver();
    }
}

bool MfrLcCommManager::connectToLc()
{
    Logger::log("[MfrLcCommManager] Attempting to connect to server...");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        Logger::log("[MfrLcCommManager] Failed to create socket");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(lcPort);
    if (inet_pton(AF_INET, lcIp.c_str(), &serverAddr.sin_addr) <= 0)
    {
        Logger::log("[MfrLcCommManager] Invalid address");
        close(sockfd);
        return false;
    }

    if (connect(sockfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        Logger::log("[MfrLcCommManager] Connection failed");
        close(sockfd);
        return false;
    }

    Logger::log("[MfrLcCommManager] Successfully connected to LC server");
    return true;
}

void MfrLcCommManager::startTcpReceiver()
{
    if (isRunning_)
    {
        Logger::log("[MfrLcCommManager] Receiver already running");
        return;
    }

    isRunning_ = true;
    receiverThread = std::thread(&MfrLcCommManager::runReceiver, this);
}

void MfrLcCommManager::runReceiver()
{
    std::vector<char> buffer(BUFFER_SIZE);
    Logger::log("[MfrLcCommManager] TCP Receiver thread started");

    while (isRunning_)
    {
        ssize_t len = read(sockfd, buffer.data(), buffer.size());
        if (len <= 0)
        {
            if (isRunning_)
            {
                Logger::log("[MfrLcCommManager] Connection terminated or receive failed");
            }
            break;
        }

        std::vector<char> packet(buffer.begin(), buffer.begin() + len);
        safeCallbackData(packet);
    }

    Logger::log("[MfrLcCommManager] Receiver thread stopped");
}

void MfrLcCommManager::safeCallbackData(const std::vector<char> &packet)
{
    std::lock_guard<std::mutex> lock(receiverMutex_);
    if (auto receiver = receiver_.lock())
    {
        try
        {
            receiver->callBackData(packet);
            Logger::log("[MfrLcCommManager] Successfully processed packet of size: " + std::to_string(packet.size()));
        }
        catch (const std::exception &e)
        {
            Logger::log("[MfrLcCommManager] Error in callback: " + std::string(e.what()));
        }
    }
}

void MfrLcCommManager::stopReceiver()
{
    isRunning_ = false;
    if (receiverThread.joinable())
    {
        receiverThread.join();
    }
}

void MfrLcCommManager::send(const std::vector<char> &packet)
{
    if (sockfd < 0)
    {
        Logger::log("[MfrLcCommManager] Error: Socket not open");
        return;
    }

    try
    {
        ssize_t sent = write(sockfd, packet.data(), packet.size());
        if (sent != static_cast<ssize_t>(packet.size()))
        {
            Logger::log("[MfrLcCommManager] Send failed: " + std::to_string(sent) + " of " + std::to_string(packet.size()));
        }
        else
        {
            Logger::log("[MfrLcCommManager] Successfully sent " + std::to_string(sent) + " bytes");
        }
    }
    catch (const std::exception &e)
    {
        Logger::log("[MfrLcCommManager] Send error: " + std::string(e.what()));
    }
}