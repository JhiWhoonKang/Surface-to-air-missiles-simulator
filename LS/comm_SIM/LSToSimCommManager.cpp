#include "LSToSimCommManager.h"
#include "ConfigParser.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

LSToSimCommManager::LSToSimCommManager(const std::string& configPath) 
{
    fd = init(configPath);
}

LSToSimCommManager::~LSToSimCommManager() 
{
    stop();
}

int LSToSimCommManager::init(const std::string& configPath) 
{
    try 
    {
        IP = ConfigParser::getValue("SimulatorComm", "IP", configPath);
        port = ConfigParser::getInt("SimulatorComm", "Port", configPath);
    }

    catch (const std::exception& e)
    {
        std::cerr << "[Config Error] " << e.what() << "\n";
        return -1;
    }

    if (port <= 0 || port > 65535)
    {
        std::cerr << "[LSToSimCommManager] Invalid port: " << port << "\n";
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket creation failed");
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, IP.c_str(), &address.sin_addr) <= 0)
    {
        std::cerr << "[LSToSimCommManager] Invalid IP address: " << IP << "\n";
        return -1;
    }

    std::cout << "[LSToSimCommManager] Initialized with IP: " << IP << ", Port: " << port << "\n";
    return fd;
}

void LSToSimCommManager::sendMissileInfo(const LaunchCommand& command, const LSStatusManager& statManager)
{
    //long long x, y, z;
    int spd; //= command.speed;
    //statManager.positionBriefing(x, y, z);

    statManager.speedBriefing(spd);
    //MissileInfo missile{x, y, z, spd, command.launch_angle_xy, command.launch_angle_xz};
    MissileInfo missile{command.start_x, command.start_y, command.start_z, spd, command.launch_angle_xy, command.launch_angle_xz};
    std::cout << "[Missile Info Packet Created]\n";
    std::cout << "  Position: (" << command.start_x << ", " << command.start_y << ", " << command.start_z << ")\n";
    std::cout << "  Speed: " << spd << "\n";
    std::cout << "  Launch Angle XY: " << command.launch_angle_xy << "\n";
    std::cout << "  Launch Angle XZ: " << command.launch_angle_xz << "\n";

    std::vector<uint8_t> packet = missile.toBytes();
    // print
    for (const auto& byte : packet)
    {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }

    MissileInfo from_missile;
    from_missile = MissileInfo::fromBytes(packet);
    // from_missile.print();
    // std::cout << "here==========" << std::endl;
    // std::cout << "[Missile Info Packet Created]\n";
    // std::cout << "  Position: (" << x << ", " << y << ", " << z << ")\n";
    // std::cout << "  Launch Angle XY: " << command.launch_angle_xy << "\n";
    // std::cout << "  Launch Angle XZ: " << command.launch_angle_xz << "\n";
    std::cout << "[Missile Info Packet Size: " << packet.size() << " bytes]\n";
    ssize_t sent = sendto(fd, packet.data(), packet.size(), 0, (sockaddr*)&address, sizeof(address));
    
    if (sent < 0)
    {
        perror("sendto failed");
    }
    
    else
    {
        std::cout << "[Missile Fire Command Sent]\n";
    }
}

void LSToSimCommManager::stop()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}
