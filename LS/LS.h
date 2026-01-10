#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "LSToSimCommManager.h" 

#include "LCToLSCommInterface.h"
#include "LCToLSCommFactory.h"
#include "SerialReceiverInterface.h"
#include "info.h"
#include "LSStatusManager.h"

class LS : public SerialReceiverInterface {
private:
    std::deque<LauncherMessage> schedule;
    std::mutex scheduleMutex;
    std::condition_variable scheduleCV; 

    std::unique_ptr<LSToSimCommManager> simManager;
    std::unique_ptr<LCToLSCommInterface> lcManager;
    std::unique_ptr<LSStatusManager> statManager;

    std::thread workerThread;
    bool stopFlag = false;

    // 초기화 메소드
    void init(const std::string& path);

    // 명령 처리 루프
    void workerLoop();
    void move(const MoveCommand& command);
    void launch(const LaunchCommand& command);
    void changeMode(const ModeChangeCommand& command);
    void sendStatus();
public:
    explicit LS(const std::string& path);
    
    // 스레드 종료
    ~LS();
    
    // LS의 메서드
    void callBack(const std::vector<uint8_t>& data) override;

};
