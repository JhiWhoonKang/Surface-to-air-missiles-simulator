#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic>

#include "IReceiver.h"
#include "IStatusSender.h"
#include "SenderType.h"
#include "SystemStatus.h"       // ✅ 이것이 반드시 필요
#include "CommonMessage.h"

class SerialLS : public IReceiver, public IStatusSender{
public:
    SerialLS(int localport, const std::string& ip, int port);  // ✅ string 타입 정확히 사용

    void setCallback(IReceiverCallback* cb);  // ✅ LCManager → IReceiverCallback

    void start() override;
    SenderType getSenderType() const override;

    // IStatusSender 인터페이스 구현
    void sendStatus(const SystemStatus& status); // 추가 함수
    void sendStatus(const Common::CommonMessage& msg) override; // 인터페이스 구현

    void sendRaw(const std::vector<uint8_t>& data) override {
        sendRaw(data, "");
    }
    void handleReceived(const std::vector<uint8_t>& data, SenderType from) override;

    void sendResponse(uint8_t radarId, uint8_t mode, bool ok, const std::string& msg);

private:
    IReceiverCallback* callback_ = nullptr;
    int sockfd_;
    uint16_t localPort_;
    std::string lcIp_;
    uint16_t lcPort_;
    std::atomic<bool> stop_;

    void sendRaw(const std::vector<uint8_t>& data, const std::string& prefix);
};
