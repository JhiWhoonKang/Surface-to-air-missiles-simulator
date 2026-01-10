#include "LS.h"
#include "ConfigParser.h"
#include "LCToLSCommUDPManager.h" // debug
#include <iostream>
#include <cstring>
#include <mutex>
#include <iomanip>

LS::LS(const std::string &mainConfigPath)
{
    init(mainConfigPath);
    workerThread = std::thread(&LS::workerLoop, this); // 명령 처리 스레드 시작
}

LS::~LS()
{
    {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        stopFlag = true;
    }
    scheduleCV.notify_all();
    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void LS::init(const std::string &mainConfigPath)
{
    try
    {
        std::string simConfigPath = ConfigParser::getValue("ConfigPath", "SimulatorConfig", mainConfigPath);
        std::string fireConfigPath = ConfigParser::getValue("ConfigPath", "FireControlConfig", mainConfigPath);
        std::string launcherConfigPath = ConfigParser::getValue("ConfigPath", "LauncherConfig", mainConfigPath);

        simManager = std::make_unique<LSToSimCommManager>(simConfigPath);
        lcManager = createLCToLSComm(*this, fireConfigPath);
        statManager = std::make_unique<LSStatusManager>(launcherConfigPath);

        std::cout << "[LS] All subsystems initialized.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[LS] Init error: " << e.what() << "\n";
    }
}

void LS::callBack(const std::vector<uint8_t> &data)
{
    std::cout << "[LS] receive() called.\n";

    if (data.size() < sizeof(CommandType) + sizeof(unsigned int))
    {
        std::cerr << "[LS] Invalid data size\n";
        return;
    }

    LauncherMessage msg{};
    size_t offset = 0;

    // 1. 명령 타입
    std::memcpy(&msg.type, data.data() + offset, sizeof(msg.type));
    offset += sizeof(msg.type);

    // 2. launcher_id
    std::memcpy(&msg.launcher_id, data.data() + offset, sizeof(msg.launcher_id));
    offset += sizeof(msg.launcher_id);

    switch (msg.type)
    {
    case CommandType::LAUNCH:
    {
        if (data.size() >= offset + sizeof(LaunchCommand))
        {
            // 각 멤버별로 파싱
            std::memcpy(&msg.launch.launch_angle_xy, data.data() + offset, sizeof(double));
            offset += sizeof(double);

            std::memcpy(&msg.launch.launch_angle_xz, data.data() + offset, sizeof(double));
            offset += sizeof(double);
            
            std::memcpy(&msg.launch.start_x, data.data() + offset, sizeof(long long));
            offset += sizeof(long long);
            std::memcpy(&msg.launch.start_y, data.data() + offset, sizeof(long long));
            offset += sizeof(long long);
            std::memcpy(&msg.launch.start_z, data.data() + offset, sizeof(long long));
            offset += sizeof(long long);

            std::cout << "\n[Launch Command]\n";
            std::cout << "  Launch Angle XY : " << msg.launch.launch_angle_xy << "\n";
            std::cout << "  Launch Angle XZ : " << msg.launch.launch_angle_xz << "\n";
            std::cout << "  start_x : " << msg.launch.start_x << "\n";
            std::cout << "  start_y : " << msg.launch.start_x << "\n";
            std::cout << "  start_z : " << msg.launch.start_x << "\n";
        }
        else
        {
            std::cerr << "[LS] LaunchCommand size mismatch\n";
            return;
        }
        break;
    }

    case CommandType::MOVE:
    {
        if (data.size() >= offset + sizeof(MoveCommand))
        {
            std::memcpy(&msg.move, data.data() + offset, sizeof(MoveCommand));
            std::cout << "\n[Move Command]\n";
            std::cout << "  new_x : " << msg.move.new_x << "\n";
            std::cout << "  new_y : " << msg.move.new_y << "\n";
        }
        else
        {
            std::cerr << "[LS] MoveCommand size mismatch\n";
            return;
        }
        break;
    }

    case CommandType::MODE_CHANGE:
    {
        if (data.size() >= offset + sizeof(ModeChangeCommand))
        {
            std::memcpy(&msg.mode_change, data.data() + offset, sizeof(ModeChangeCommand));
            std::cout << "\n[Mode Change Command]\n";
            std::cout << "  New Mode : " << static_cast<int>(msg.mode_change.new_mode) << "\n";
        }
        else
        {
            std::cerr << "[LS] ModeChangeCommand size mismatch\n";
            return;
        }
        break;
    }

    case CommandType::STATUS_REQUEST:
    {
        std::cout << "\n[Status Request Received]\n";
        break;
    }

    default:
    {
        std::cerr << "[LS] Unknown command type\n";
        return;
    }
    }

    // lock_guard를 이용해 스케쥴 버퍼 mutex_lock
    {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        schedule.push_back(msg);
    }
    scheduleCV.notify_one(); // 스레드 깨우기
}

void LS::launch(const LaunchCommand &command)
{
    if (simManager && statManager)
    {
        
        if (!statManager->rotateToAngle(command.launch_angle_xy))
        {
            std::cerr << "[LS] Failed to rotate to launch angle\n";
            return;
        }
        
        simManager->sendMissileInfo(command, *statManager);
    }
    else
    {
        std::cerr << "[LS] SimManager or StatusManager not initialized.\n";
    }
}

void LS::move(const MoveCommand &command)
{
    if (statManager)
    {
        statManager->moveLS(command.new_x, command.new_y);
    }
    else
    {
        std::cerr << "[LS] StatusManager not initialized.\n";
    }
}

void LS::changeMode(const ModeChangeCommand &command)
{
    if (statManager)
    {
        statManager->changeMode(command.new_mode);
    }
    else
    {
        std::cerr << "[LS] StatusManager not initialized.\n";
    }
}

void LS::sendStatus()
{
    std::vector<uint8_t> packet;
    statManager->serializeStatus(packet);

    lcManager->sendData(packet);
    return;
}

void LS::workerLoop()
{
    while (true)
    {
        LauncherMessage msg;
        {
            std::unique_lock<std::mutex> lock(scheduleMutex);
            scheduleCV.wait(lock, [this]
                            { return !schedule.empty() || stopFlag; });

            if (stopFlag && schedule.empty())
            {
                break;
            }

            msg = schedule.front();
            schedule.pop_front();
        }

        // 실제 명령 처리
        switch (msg.type)
        {
        case CommandType::LAUNCH:
        {
            launch(msg.launch);
            break;
        }
        case CommandType::MOVE:
        {
            move(msg.move);
            break;
        }
        case CommandType::MODE_CHANGE:
        {
            changeMode(msg.mode_change);
            break;
        }
        case CommandType::STATUS_REQUEST:
        {
            sendStatus();
            break;
        }

        default:
            std::cerr << "Unknown command in worker thread\n";
            break;
        }
    }
}