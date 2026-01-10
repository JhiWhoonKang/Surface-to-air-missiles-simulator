// TcpMFR.cpp
#include "TcpMFR.h"
#include "LCManager.h"
#include "Serializer.h"
#include "MessageParser.h"
#include <iostream>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include "Serializer.h" // serializeMessage 사용 시 필요
#include <cstring>

TcpMFR::TcpMFR(const std::string &ip, int port)
    : ip_(ip), port_(port) {}

void TcpMFR::setCallback(IReceiverCallback *cb)
{
    callback_ = cb;
}

void TcpMFR::handleReceived(const std::vector<uint8_t> &data, SenderType from)
{
    if (callback_)
    {
        callback_->onMessage(Common::MessageParser::parse(data, from));
    }
}

void TcpMFR::sendStatus(const Common::CommonMessage &msg)
{
    auto data = Common::Serializer::serializeMessage(msg);
    sendRaw(data, "[TcpMFR] CommonMessage 전송");
}

void TcpMFR::start()
{
    std::cout << "[TcpMFR] start() 진입함" << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("[TcpMFR] socket");
        return;
    }

    std::cout << "[TcpMFR] socket 생성 성공" << std::endl;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("[TcpMFR] setsockopt");
        close(server_fd);
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr) <= 0)
    {
        perror("[TcpMFR] inet_pton");
        close(server_fd);
        return;
    }

    std::cout << "[TcpMFR] bind() 시도 중" << std::endl;
    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[TcpMFR] bind");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("[TcpMFR] listen");
        close(server_fd);
        return;
    }

    std::cout << "[TcpMFR] 클라이언트 대기 중: " << ip_ << ":" << port_ << std::endl;
    std::thread([this, server_fd]()
                {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        sock_fd_ = accept(server_fd, (sockaddr*)&client_addr, &len);
        close(server_fd);

        if (sock_fd_ < 0) {
            perror("[TcpMFR] accept");
            return;
        }

        std::cout << "[TcpMFR] 클라이언트 연결됨" << std::endl;
        receiveLoop(); })
        .detach();

    // std::cout << "[TcpMFR] 클라이언트 연결됨" << std::endl;
    // std::thread(&TcpMFR::receiveLoop, this).detach();   // 146420
}

void TcpMFR::receiveLoop()
{
    std::cout << "[TcpMFR] receiveLoop() 진입\n";

    while (true)
    {
        uint8_t buffer[3072];
        ssize_t len = recv(sock_fd_, buffer, sizeof(buffer), 0);
        // std::cout << "[TcpMFR] recv() 호출됨, len = " << len << "\n";

        if (len == 0)
        {
            // std::cerr << "[TcpMFR] 클라이언트가 연결을 종료했습니다.\n";
            break;
        }
        else if (len < 0)
        {
            std::cerr << "[TcpMFR] 수신 에러: " << strerror(errno) << " (errno=" << errno << ")\n";
            break;
        }

        // std::cout << "[TcpMFR] 데이터 수신 성공: " << len << " 바이트\n";

        std::vector<uint8_t> raw(buffer, buffer + len);

        Common::CommonMessage msg;
        try
        {
            msg = Common::MessageParser::parse(raw, getSenderType());
            // std::cout << "[TcpMFR] 파싱 성공: CommandType = " << static_cast<int>(msg.commandType) << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "[TcpMFR] 파싱 중 예외: " << e.what() << "\n";
            continue;
        }
        if (callback_)
        {
            // std::cout << "[TcpMFR] onMessage 호출\n";
            callback_->onMessage(msg);
        }
    }

    std::cerr << "[TcpMFR] 수신 루프 종료, 소켓 닫음\n";
    close(sock_fd_);
}

SenderType TcpMFR::getSenderType() const
{
    return SenderType::MFR;
}

void TcpMFR::sendStatus(const SystemStatus &status)
{
    auto data = Common::Serializer::serializeStatusResponse(status);
    sendRaw(data, "[TcpMFR] 상태 전송");
}

// void TcpMFR::sendResponse(uint8_t radarId, uint8_t mode, bool ok, const std::string& msg) {
//     auto data = Common::Serializer::serializeRadarResponse(radarId, mode, ok, msg);
//     sendRaw(data, "[TcpMFR] 응답 전송");
// }

void TcpMFR::sendRaw(const std::vector<uint8_t> &data)
{
    static int sendCounter2 = 0;
    sendCounter2++;

    // const std::string prefix = "[TcpMFR] 일반 전송";

    ssize_t sent = send(sock_fd_, data.data(), data.size(), 0);
    if (sent < 0)
    {
        // std::cerr << prefix << " - 전송 실패 (errno=" << errno << ")\n";
    }
    else
    {
        if (sendCounter2 % 10 == 0)
        {
            // std::cout << prefix << " - " << sent << " 바이트 전송 완료\n";
        }
    }
}
void TcpMFR::sendRaw(const std::vector<uint8_t> &data, const std::string &prefix)
{
    static int sendCounter = 0;
    sendCounter++;

    ssize_t sent = send(sock_fd_, data.data(), data.size(), 0);
    if (sent < 0)
    {
        std::cerr << prefix << " - 전송 실패 (errno=" << errno << ")\n";
    }
    else
    {
        if (sendCounter % 10 == 0)
        {
            // std::cout << prefix << " - " << sent << " 바이트 전송 완료\n";
        }
    }
}