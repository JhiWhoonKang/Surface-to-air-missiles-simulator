// #pragma once

// #include "IReceiver.h"
// #include "IStatusSender.h"
// #include "SystemStatus.h"
// #include "SenderType.h"
// #include "CommonMessage.h"
// #include <string>
// #include <vector>

// class SerialLS : public IReceiver, public IStatusSender {
// public:
//     explicit SerialLS(const std::string& port);   // ✅ 다시 시리얼 포트 기반

//     // 수신 인터페이스
//     void start() override;
//     SenderType getSenderType() const override;
//     void setCallback(IReceiverCallback* cb) override;
//     void handleReceived(const std::vector<uint8_t>& data, SenderType from) override;

//     // 송신 인터페이스
//     void sendStatus(const Common::CommonMessage& msg) override;
//     void sendRaw(const std::vector<uint8_t>& data) override;

//     void sendStatus(const SystemStatus& status);
//     void sendResponse(uint8_t lsId, uint8_t mode, bool ok, const std::string& msg);

// private:
//     std::string port_;                           // ✅ 포트 이름 사용
//     IReceiverCallback* callback_ = nullptr;
//     int openPort() const;
//     void receiveLoop();
//     void sendRaw(const std::vector<uint8_t>& data, const std::string& logPrefix);
// };