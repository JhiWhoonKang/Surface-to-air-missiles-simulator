#pragma once
#include "IReceiver.h"
#include "MfrConfig.h"
#include <vector>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>

class MfrSimCommManager
{
private:
    std::weak_ptr<IReceiver> receiver_; // weak_ptr로 변경
    int sockfd;
    int simPort;
    std::atomic<bool> isRunning_;
    std::thread receiverThread; // 스레드 객체 추가
    std::mutex callbackMutex_; // 스레드 안전을 위한 뮤텍스

    void processReceivedData(const char *buffer, size_t len);
    void processTargetData(const char *buffer, size_t len);
    void processMissileData(const char *buffer, size_t len);
    void runReceiver(); // 실제 수신 작업을 수행할 메서드

public:
    explicit MfrSimCommManager(std::shared_ptr<IReceiver> receiver);
    ~MfrSimCommManager();
    MfrSimCommManager(const MfrSimCommManager &) = delete;
    MfrSimCommManager &operator=(const MfrSimCommManager &) = delete;

    void initMfrSimCommManager();
    bool connectToSim();
    void startUdpReceiver();
    void stopReceiver();
};