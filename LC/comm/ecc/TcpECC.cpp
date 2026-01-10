#include "TcpECC.h"
#include "LCManager.h"
#include "Serializer.h"
#include "MessageParser.h"
#include "IReceiverCallback.h"
#include "IReceiver.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

TcpECC::TcpECC(const std::string& ip, int port)
    : ip_(ip), port_(port) {}

void TcpECC::setCallback(IReceiverCallback* cb) {
    callback_ = cb;
}

void TcpECC::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[TcpECC] socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0 || listen(server_fd, 5) < 0) {
        perror("[TcpECC] bind/listen");
        close(server_fd);
        return;
    }

    std::cout << "[TcpECC] 클라이언트 대기 중: " << ip_ << ":" << port_ << std::endl;

    std::thread([this, server_fd]() {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        sock_fd_ = accept(server_fd, (sockaddr*)&client_addr, &len);
        close(server_fd);

        if (sock_fd_ < 0) {
            perror("[TcpECC] accept");
            return;
        }

        // std::cout << "[TcpECC] 클라이언트 연결됨" << std::endl;
        receiveLoop();
    }).detach();
}

void TcpECC::receiveLoop() {
    while (true) {
        uint8_t buffer[3072];
        ssize_t len = recv(sock_fd_, buffer, sizeof(buffer), 0);
        if (len <= 0) break;

        std::vector<uint8_t> raw(buffer, buffer + len);
        auto msg = Common::MessageParser::parse(raw, getSenderType());

        if (callback_) {
            callback_->onMessage(msg);
        }
    }
}

SenderType TcpECC::getSenderType() const {
    return SenderType::ECC;
}

void TcpECC::handleReceived(const std::vector<uint8_t>& data, SenderType from) {
    if (callback_) {
        auto msg = Common::MessageParser::parse(data, from);
        callback_->onMessage(msg);
    }
}

void TcpECC::sendStatus(const SystemStatus& status) {
    auto data = Common::Serializer::serializeStatusResponse(status);
    sendRaw(data, "[TcpECC] 상태 전송");
}

void TcpECC::sendSystemStatus(const SystemStatus& status) {
    auto data = Common::Serializer::serializeStatusResponse(status);
    sendRaw(data, "[TcpECC] 상태 전송");
}

void TcpECC::sendStatus(const Common::CommonMessage& msg) {
    auto data = Common::Serializer::serializeMessage(msg);
    sendRaw(data, "[TcpECC] 상태 전송 (CommonMessage)");
}

void TcpECC::sendResponse(uint8_t eccId, uint8_t mode, bool ok, const std::string& msg) {
    auto data = Common::Serializer::serializeConsoleResponse(eccId, mode, ok, msg);
    sendRaw(data, "[TcpECC] 응답 전송");
}

void TcpECC::sendRaw(const std::vector<uint8_t>& data, const std::string& prefix) {
    static int sendCounter = 0;
    sendCounter++;



    ssize_t sent = send(sock_fd_, data.data(), data.size(), 0);

    if (sendCounter % 10 != 0) {
        return;
    }
    if (sent < 0) {
        std::cerr << prefix << " - 전송 실패 (errno=" << errno << ")\n";
    } else {
        std::cout << prefix << " - " << sent << " 바이트 전송 완료\n";
    }
}
