#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

#include "MotorManagerInterface.h"

class LSMotorManager : public MotorManagerInterface
{
private:
    // int uart_fd = -1;

    // std::string device;
    // unsigned int uartBaudRate;
    // bool initUart();
    void connectToServer();
    int sock_fd = -1;

    std::string IP;
    int port;

public:
    LSMotorManager(const std::string &IP, const int& port);
    ~LSMotorManager();
    bool rotateToAngle(double targetAngle);
};
