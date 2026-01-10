#include "SerialLS.h"
#include "LCManager.h"
#include "Serializer.h"
#include "MessageParser.h"
#include <iostream>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include "Serializer.h"  // serializeMessage 사용 시 필요
#include <cstring>



SerialLS::SerialLS(int localport, const std::string& ip, int port)
    : localPort_(localport), lcIp_(ip), lcPort_(port), stop_(false) {
        std::cout << "[SerialLS] 생성자 호출됨, 로컬 포트: " << localPort_ << ", LC IP: " << lcIp_ << ", LC 포트: " << lcPort_ << std::endl;
    }

void SerialLS::setCallback(IReceiverCallback* cb) {
    callback_ = cb;  // SerialLS는 콜백을 사용하지 않습니다.
}


void SerialLS::handleReceived(const std::vector<uint8_t>& data, SenderType from) {
    if (callback_) {
        auto parsed = Common::MessageParser::parse(data, from);
        callback_->onMessage(parsed);
    } else {
        std::cerr << "[SerialLS] ⚠️ callback이 설정되지 않았습니다.\n";
    }
}

void SerialLS::sendStatus(const Common::CommonMessage& msg) {
    auto data = Common::Serializer::serializeMessage(msg);
    sendRaw(data, "[SerialLS] CommonMessage 전송");
}

void SerialLS::start() {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        perror("[SerialLS] socket");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(localPort_);

    if (bind(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[SerialLS] bind");
        close(sockfd_);
        return;
    }

    std::thread([this]() {
        int recvCounter = 0;  // ✅ 쓰레드 안에서 선언된 로컬 카운터

        while (!stop_) {
            uint8_t buffer[3072];
            sockaddr_in client_addr{};
            socklen_t len = sizeof(client_addr);

            ssize_t recv_len = recvfrom(sockfd_, buffer, sizeof(buffer), 0,
                                        (sockaddr*)&client_addr, &len);
            if (recv_len > 0) {
                recvCounter++;  // ✅ 정상 수신 횟수 증가

                if (recvCounter % 10 == 0) {
                    std::cout << "[SerialLS] 수신 데이터 크기: " << recv_len << " 바이트\n";
                }

                std::vector<uint8_t> raw(buffer, buffer + recv_len);
                try {
                    auto msg = Common::MessageParser::parse(raw, getSenderType());
                    if (callback_) {
                        callback_->onMessage(msg);
                    } else {
                        std::cerr << "[SerialLS] ⚠️ 콜백이 설정되지 않았습니다.\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[SerialLS] 파싱 실패: " << e.what() << "\n";
                }
            } else if (recv_len < 0) {
                std::cerr << "[SerialLS] 수신 에러: " << strerror(errno) << "\n";
                break;
            }
        }

        close(sockfd_);
    }).detach();
}
SenderType SerialLS::getSenderType() const {
    return SenderType::LS;
}

void SerialLS::sendStatus(const SystemStatus& status) {
    auto data = Common::Serializer::serializeStatusResponse(status);
    sendRaw(data, "[SerialLS] 상태 전송");
}

// void SerialLS::sendResponse(uint8_t radarId, uint8_t mode, bool ok, const std::string& msg) {
//     auto data = Common::Serializer::serializeRadarResponse(radarId, mode, ok, msg);
//     sendRaw(data, "[SerialLS] 응답 전송");
// }

void SerialLS::sendRaw(const std::vector<uint8_t>& data, const std::string& prefix) {
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(lcPort_);
    for (const auto& byte : data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    if (inet_pton(AF_INET, lcIp_.c_str(), &dest_addr.sin_addr) <= 0) {
        perror("[SerialLS] inet_pton error");
        return;
    }

    ssize_t sent = sendto(sockfd_, data.data(), data.size(), 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent < 0) {
        std::cerr << prefix << " - 전송 실패 (errno=" << errno << "): " << strerror(errno) << "\n"; // Debugging
    } else {
        // std::cout << prefix << " - " << sent << " 바이트 전송 완료\n";
    }
}