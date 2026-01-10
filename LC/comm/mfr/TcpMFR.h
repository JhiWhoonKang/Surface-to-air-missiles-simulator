#pragma once
#include "IReceiver.h"
#include "IStatusSender.h"
#include "SenderType.h"
#include "SystemStatus.h"       // ✅ 이것이 반드시 필요
#include "CommonMessage.h"

#include <string>
#include <vector>


class TcpMFR : public IReceiver, public IStatusSender {
public:
    TcpMFR(const std::string& ip, int port);  // ✅ string 타입 정확히 사용

    void setCallback(IReceiverCallback* cb);  // ✅ LCManager → IReceiverCallback

    void start() override;
    SenderType getSenderType() const override;

    // IStatusSender 인터페이스 구현
    void sendStatus(const SystemStatus& status); // 추가 함수
    void sendStatus(const Common::CommonMessage& msg) override; // 인터페이스 구현

    void sendRaw(const std::vector<uint8_t>& data) override;
    void handleReceived(const std::vector<uint8_t>& data, SenderType from) override;

    void sendResponse(uint8_t radarId, uint8_t mode, bool ok, const std::string& msg);

private:
    std::string ip_;
    int port_;
    int sock_fd_ = -1;
    IReceiverCallback* callback_ = nullptr;

    void receiveLoop();
    void sendRaw(const std::vector<uint8_t>& data, const std::string& prefix);
};