#pragma once
#include "LSStatus.h"
#include "LSMotorManager.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class LSStatusManager
{
private:
    LSStatus status;

    std::unique_ptr<MotorManagerInterface> motorManager;

    std::thread moveThread;
    std::atomic<bool> moveFlag{false};
    std::mutex moveMutex;
    std::condition_variable moveCV; 

    std::mutex threadJoinMutex;
    void init(const std::string& launcherConfigPath);
    void moveRoutine(long long destX, long long destY);
    void updatePosition(long long x, long long y);
    void updateLaunchAngle(double angle);

public:
    // 기본 생성자 및 초기화
    LSStatusManager(const std::string& launcherConfigPath);
    ~LSStatusManager();

    // 책임 메서드들    
    void changeMode(OperationMode mode);
    void positionBriefing(long long& x, long long& y, long long& z) const;
    void serializeStatus(std::vector<uint8_t>& out) const;
    void moveLS(long long x, long long y);
    bool rotateToAngle(double angle);
    void speedBriefing(int& speed) const;
};