#include "LCToLSCommUDPManager.h"
#include "ConfigParser.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "info.h"

LCToLSUDPCommManager::LCToLSUDPCommManager(SerialReceiverInterface& receiver, const std::string& configPath)
    : receiver(receiver)
{
    init(configPath);
    listenerThread = std::thread(&LCToLSUDPCommManager::run, this);
}

LCToLSUDPCommManager::~LCToLSUDPCommManager()
{
    if (listenerThread.joinable())
        listenerThread.join();
    
    if (recvSock >= 0)
        close(recvSock);
    if (sendSock >= 0)
        close(sendSock);
}

void LCToLSUDPCommManager::init(const std::string& configPath)
{
        try {
        std::string localPortStr = ConfigParser::getValue("FireControlCommUDP", "LocalPort", configPath);
        std::string remoteIP     = ConfigParser::getValue("FireControlCommUDP", "RemoteIP", configPath);
        std::string remotePortStr= ConfigParser::getValue("FireControlCommUDP", "RemotePort", configPath);

        int listenPort = std::stoi(localPortStr);
        int sendPort = std::stoi(remotePortStr);
        remotePort = sendPort;

        // 수신 소켓(recvSock) 생성 및 바인딩
        recvSock = socket(AF_INET, SOCK_DGRAM, 0);
        if (recvSock < 0) {
            perror("[LCToLSUDPCommManager] recvSock creation failed");
            throw std::runtime_error("recvSock creation failed");
        }

        memset(&localAddr, 0, sizeof(localAddr));
        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = INADDR_ANY;
        localAddr.sin_port = htons(listenPort);

        if (bind(recvSock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
            perror("[LCToLSUDPCommManager] recvSock bind failed");
            close(recvSock);
            recvSock = -1;
            throw std::runtime_error("recvSock bind failed");
        }

        memset(&remoteAddr, 0, sizeof(remoteAddr));
        remoteAddr.sin_family = AF_INET;
        remoteAddr.sin_port = htons(sendPort);
        inet_pton(AF_INET, remoteIP.c_str(), &remoteAddr.sin_addr);

        std::cout << "[LCToLSUDPCommManager] UDP initialized. Listening on port " << listenPort 
                  << ", sending to " << remoteIp << ":" << sendPort << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[LCToLSUDPCommManager] Config error: " << e.what() << "\n";
        if (recvSock >= 0) close(recvSock);
        if (sendSock >= 0) close(sendSock);
        recvSock = -1;
        sendSock = -1;
    }
}

void LCToLSUDPCommManager::run()
{
    if (recvSock < 0) {
        std::cerr << "[LCToLSUDPCommManager] Invalid socket\n";
        return;
    }

    std::cout << "[LCToLSUDPCommManager] Listening for UDP packets...\n";

    while (true) {
        std::vector<uint8_t> buffer(sizeof(LauncherMessage));
        sockaddr_in senderAddr {};
        socklen_t addrLen = sizeof(senderAddr);
        ssize_t len = recvfrom(recvSock, buffer.data(), buffer.size(), 0,
                               (struct sockaddr*)&senderAddr, &addrLen);

        if (len > 0) {
            std::cout << "[LCToLSUDPCommManager] Received " << len << " bytes from "
                      << inet_ntoa(senderAddr.sin_addr) << ":" << ntohs(senderAddr.sin_port) << "\n";
            receiver.callBack(buffer);
        } else if (len < 0) {
            perror("[LCToLSUDPCommManager] recvfrom error");
        }
    }
}

void LCToLSUDPCommManager::sendData(const std::vector<uint8_t>& packet)
{
    ssize_t sent = sendto(recvSock, packet.data(), packet.size(), 0,
                          (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
    if (sent < 0) {
        perror("[LCToLSUDPCommManager] sendto error");
    } else {
        std::cout << "[LCToLSUDPCommManager] Sent " << sent << " bytes via UDP\n";
    }
}
