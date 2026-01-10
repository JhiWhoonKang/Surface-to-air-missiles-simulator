#include "pch.h"
#include "ECC_TCP.h"
#include "Config.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <iostream>
#include <vector>
#include "MAP_TCP.h"
ECC_TCP::ECC_TCP() {}

ECC_TCP::~ECC_TCP() {
    stop();
}

bool ECC_TCP::connect(const char* ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }

    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sock == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    if (::connect(m_sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(m_sock);
        WSACleanup();
        return false;
    }

    ConfigCommon config;
    if (!loadConfig("ECCconfig.ini", config))
    {
      AfxMessageBox(_T("���� ������ �о���� �� �����߽��ϴ�."));
      return FALSE;  // �ʱ�ȭ ����
    }

    // Map Connect
    map_tcp = std::make_unique<MAP_TCP>();
    if (map_tcp->connect(config.MAPSendIP, config.MAPSendPort))
    {
      std::cout << "success Map TCP Connect" << std::endl;
    }

    return true;
}

void ECC_TCP::registerReceiver(IReceiver* receiver)
{
    m_receiver = receiver;
}

void ECC_TCP::send(const char* data, int len)
{
    if (m_sock != INVALID_SOCKET)
    {
        ::send(m_sock, data, len, 0);
    }
}

void ECC_TCP::startReceiving()
{
    if (!m_bRunning)
    {
        m_bRunning = true;
        m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, recvThread, this, 0, nullptr));
    }
}

unsigned __stdcall ECC_TCP::recvThread(void* arg) {
    ECC_TCP* self = static_cast<ECC_TCP*>(arg);
    char tempBuf[4096];
    std::vector<uint8_t> buffer;  // 누적 수신 버퍼

    while (self->m_bRunning)
    {
        int len = recv(self->m_sock, tempBuf, sizeof(tempBuf), 0);
        if (len <= 0) break;  // 소켓 종료 또는 에러

        // 1. 수신된 데이터 누적
        buffer.insert(buffer.end(), tempBuf, tempBuf + len);

        // 2. 가능한 만큼 완전한 메시지를 처리
        while (true)
        {
            if (buffer.size() < 5)
            {
                break;  // 최소 헤더(1+4) 미만이면 break
            }

            // 싱크 맞추기
            if (buffer[0] != 0x51)
            {
                buffer.erase(buffer.begin());
                continue;
            }

            // 메시지 길이 확인
            uint32_t payloadSize;
            std::memcpy(&payloadSize, &buffer[1], 4);
            size_t totalMsgSize = 1 + 4 + payloadSize;

            // 전체 메시지가 도착하지 않았다면 대기
            if (buffer.size() < totalMsgSize)
            {
                break;
            }             

            // 헤더 다음 3바이트 (radar/lc/ls flag) 검증
            if (totalMsgSize < 8 ||  // 1+4+3보다 작다면 구조 자체가 이상함
                buffer[5] != 0x01 || buffer[6] != 0x01 || buffer[7] != 0x01) {
                std::cout << "[WARN] Invalid radar/lc/ls header flags. Dropping message.\n";
                buffer.erase(buffer.begin(), buffer.begin() + totalMsgSize);
                continue;
            }

            // 메시지 유효, 처리
            const uint8_t* msgData = buffer.data();
            size_t msgLen = totalMsgSize;

            if (self->m_receiver)
            {
                self->m_receiver->receive(msgLen, reinterpret_cast<const char*>(msgData));
            }

            if (self->map_tcp)
            {
                self->map_tcp->send(reinterpret_cast<const char*>(msgData), msgLen);
            }

            // 처리한 메시지를 버퍼에서 제거
            buffer.erase(buffer.begin(), buffer.begin() + totalMsgSize);
        }
    }

    return 0;
}

void ECC_TCP::stop() {
    m_bRunning = false;

    if (m_sock != INVALID_SOCKET) {
        shutdown(m_sock, SD_BOTH);  // ���� ����
        closesocket(m_sock);
        m_sock = INVALID_SOCKET;
    }

    if (m_hThread) {
        WaitForSingleObject(m_hThread, 1000); // �ִ� 1�� ���
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }

    WSACleanup();
}
