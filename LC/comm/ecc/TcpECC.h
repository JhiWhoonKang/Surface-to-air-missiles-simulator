#pragma once

#include "IStatusSender.h"
#include "IReceiver.h"
#include "SystemStatus.h"
#include "SenderType.h"
#include "CommonMessage.h"
#include "Serializer.h"
#include <vector>
#include <string>
#include <memory>

class LCManager;

class TcpECC : public IReceiver, public IStatusSender {
public:
    TcpECC(const std::string& ip, int port);

    void setCallback(IReceiverCallback* cb);
    void start() override;
    SenderType getSenderType() const override;

    // IStatusSender 인터페이스 구현
    void sendStatus(const SystemStatus& status);
    void sendRaw(const std::vector<uint8_t>& data) override {
        sendRaw(data, "");
    }

    // TcpECC만의 기능
    void sendStatus(const Common::CommonMessage& msg) override;
    void sendResponse(uint8_t eccId, uint8_t mode, bool ok, const std::string& msg);
    void sendSystemStatus(const SystemStatus& status);
    void handleReceived(const std::vector<uint8_t>& data, SenderType from) override;

    static std::vector<uint8_t> serializeMessage(const Common::CommonMessage& msg);

private:
    std::string ip_;
    int port_;
    int sock_fd_ = -1;
    IReceiverCallback* callback_ = nullptr;

    void receiveLoop();
    void sendRaw(const std::vector<uint8_t>& data, const std::string& prefix);
};
