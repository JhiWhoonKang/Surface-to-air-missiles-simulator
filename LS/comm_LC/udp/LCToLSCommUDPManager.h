#pragma once

#include <string>
#include <vector>
#include <thread>
#include <netinet/in.h>
#include "SerialReceiverInterface.h"
#include "LCToLSCommInterface.h"
class LCToLSUDPCommManager : public LCToLSCommInterface
{
private:
    int recvSock = -1;
    int sendSock = -1;
    std::string remoteIp;
    int remotePort = 0;
    std::thread listenerThread;
    sockaddr_in remoteAddr{}, localAddr{};
    SerialReceiverInterface& receiver;

    void run();
    void init(const std::string& configPath);

public:
    LCToLSUDPCommManager(SerialReceiverInterface& receiver, const std::string& configPath);
    ~LCToLSUDPCommManager();

    void sendData(const std::vector<uint8_t>& packet);
};
