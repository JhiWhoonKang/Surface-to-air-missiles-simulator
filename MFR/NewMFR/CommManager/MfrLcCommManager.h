#pragma once
#include "IReceiver.h"
#include "MfrConfig.h"

#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <atomic>

class MfrLcCommManager
{
private:
    const unsigned int mfrId = 1;
    int sockfd;
    std::weak_ptr<IReceiver> receiver_; // weak_ptr로 변경
    std::string lcIp;
    int lcPort;
    std::mutex receiverMutex_;
    std::atomic<bool> isRunning_{false}; // 스레드 제어용 추가
    std::thread receiverThread;          // 수신 스레드 관리용 추가

    void safeCallbackData(const std::vector<char> &packet);

public:
    explicit MfrLcCommManager(std::shared_ptr<IReceiver> receiver);
    ~MfrLcCommManager();

    MfrLcCommManager(MfrLcCommManager &&) noexcept = default;
    MfrLcCommManager &operator=(MfrLcCommManager &&) noexcept = default;
    MfrLcCommManager(const MfrLcCommManager &) = delete;
    MfrLcCommManager &operator=(const MfrLcCommManager &) = delete;

    void send(const std::vector<char> &packet);

private:
    void initMfrLcCommManager();
    bool connectToLc();
    void startTcpReceiver();
    void stopReceiver(); // 스레드 정리용 메서드 추가
    void runReceiver();  // 실제 수신 작업을 수행할 메서드
};