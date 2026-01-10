#pragma once

#include "PacketProtocol.h"
#include "MfrLcCommManager.h"
#include "StepMotorController.h"
#include "MfrSimCommManager.h"

#include <thread>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <map>

class Mfr : public IReceiver, public std::enable_shared_from_this<Mfr>
{
    // const data
private:
    const unsigned int mfrId = 101001;
    const long long limitDetectionRange = 1000000;
    const Pos3D mfrCoords = {7.5481160, 126.9961166, 244.0};
    const double EARTH_RADIUS_M = 6'371'000.0; // 지구 반지름 (m)
    const double SCALE = 1e7;

    bool motorRotationFlag = false;

    // shared data
private:
    std::map<unsigned int, localMockSimData> mockTargets;
    std::unordered_map<unsigned int, localMockSimData> mockMissile;

    std::unordered_map<unsigned int, localMockSimData> detectedTargets;
    std::unordered_map<unsigned int, localMockSimData> detectedMissile;

    void addMockMissile(const localMockSimData &missile);
    void addMockTarget(const localMockSimData &target);
    void requestLcInitData();

    // pointers to managers
private:
    std::shared_ptr<MfrLcCommManager> lcCommManager = nullptr;
    std::unique_ptr<StepMotorController> stepMotorManager = nullptr;
    std::shared_ptr<MfrSimCommManager> simCommManager = nullptr;

private:
    unsigned int goalTargetId;

    RadarMode mfrMode;
    double goalMotorAngle;

    std::atomic<bool> detectionThreadRunning{false};
    std::thread detectionThread;
    Pos3D lcCoords;

    double deg2rad(const double &deg);
    double calcDistance(const Pos3D &mfrCoord, const Pos3D &mockCoord);
    double calcBearing(const Pos3D &mfrCoord, const Pos3D &mockCoord);
    EncodedPos3D encode(const Pos3D &p);
    Pos3D decode(const EncodedPos3D &e);
    unsigned long toEpochMillis(const std::chrono::system_clock::time_point &tp);
    std::vector<char> serializeDetectionPacket(const std::vector<MfrToLcTargetInfo> &targets, const std::vector<MfrToLcMissileInfo> &missiles);
    double angleDiff(const double &baseAngle, const double &targetAngle);

    template <typename T>
    std::vector<char> serializePacketforSend(const T &status);

    void parsingSimData(const std::vector<char> &payload);
    void sendMfrStatus();
    void parsingModeChangeData(const std::vector<char> &payload);
    void parsingLcInitData(const std::vector<char> &payload);

public:
    Mfr() : goalTargetId(101001), mfrMode(ROTATION_MODE), goalMotorAngle(135.0) {}

    void initialize()
    {
        stepMotorManager = std::make_unique<StepMotorController>();
        lcCommManager = std::make_shared<MfrLcCommManager>(shared_from_this());
        simCommManager = std::make_shared<MfrSimCommManager>(shared_from_this());
        startDetectionAlgoThread();
        requestLcInitData();
    };

    ~Mfr()
    {
        // 모든 스레드가 종료될 때까지 대기
        if (simCommManager)
        {
            simCommManager->stopReceiver();
        }
    };

    void startDetectionAlgoThread();
    void mfrDetectionAlgo();
    void stopDetectionThread();

    void callBackData(const std::vector<char> &packet) override;
};