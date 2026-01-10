#include "LSMotorManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <iomanip>
LSMotorManager::LSMotorManager(const std::string& IP, const int& port)
    : MotorManagerInterface(), port(port), IP(IP)
{
    connectToServer();
}

LSMotorManager::~LSMotorManager()
{
    if (sock_fd >= 0)
    {
        close(sock_fd);
    }
}

void LSMotorManager::connectToServer()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        std::cerr << "[TCP] Socket creation failed\n";
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, IP.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "[TCP] Invalid IP address: " << IP << "\n";
    }

    std::cout << "[TCP] Trying to connect to " << IP << ":" << port << "...\n";

    int connect_count = 5;
    for (int i = 0; i < connect_count; ++i)
    {
        if (sock_fd < 0)
        {
            std::cerr << "[TCP] Socket not created\n";
            return;
        }

        if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0)
        {
            std::cout << "[TCP] Connected to server\n";
            return;
        }

        std::cerr << "[TCP] Connection failed. Retrying in 1 second...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool LSMotorManager::rotateToAngle(double targetAngle)
{
    if (sock_fd < 0)
    {
        std::cerr << "[UART] UART not initialized" << std::endl;
        return false;
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << targetAngle;
    std::string cmd = "STOP_MODE:" + oss.str() + "\n";
    ssize_t sent = send(sock_fd, cmd.c_str(), cmd.length(), 0);
    if (sent < 0)
    {
        std::cerr << "[TCP] Failed to send command\n";
        return false;
    }
    return true;
}