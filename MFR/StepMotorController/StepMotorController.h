#pragma once
#include "PacketProtocol.h"

#include "MfrConfig.h"

class StepMotorController
{
private:
    int uart_fd = -1;

    std::string device;
    speed_t uartBaudRate;

public:
    StepMotorController();
    ~StepMotorController();
    void sendCommand(const std::string &cmd);

private:
    bool initUart();
};