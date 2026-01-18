#include "pch.h"

#include "MAP_TCP.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <iostream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

MAP_TCP::MAP_TCP() : m_socket(INVALID_SOCKET) {

}

MAP_TCP::~MAP_TCP() {
    disconnect();
}

bool MAP_TCP::connect(const std::string& ip, uint16_t port) {
  disconnect();  // 이미 열려있으면 닫기

  m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_socket == INVALID_SOCKET) {
    std::cerr << "소켓 생성 실패" << std::endl;
    return false;
  }

  sockaddr_in serverAddr;
  std::memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
    std::cerr << "IP 주소 변환 실패" << std::endl;
    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
    return false;
  }

  if (::connect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr),
                sizeof(serverAddr)) == SOCKET_ERROR) {
    std::cerr << "서버 연결 실패" << std::endl;
    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
    return false;
  }

  return true;
}

int MAP_TCP::getSocket() const { return static_cast<int>(m_socket); }

void MAP_TCP::disconnect() {
  if (m_socket != INVALID_SOCKET) {
    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
  }
}

int MAP_TCP::send(const char* data, int len) {
    if(m_socket == INVALID_SOCKET)
    {
        return -1;
    }

    int sent = ::send(m_socket, data, len, 0);

    if (sent == SOCKET_ERROR) {
        // 윈도우에서는 WSAGetLastError(), 리눅스에서는 errno를 사용
        int errorCode = WSAGetLastError();
        std::cerr << "[MAP_TCP] 데이터 전송 실패. Error Code: " << errorCode << std::endl;

        // 연결이 끊긴 것으로 간주하고 소켓 초기화 등의 처리를 여기서 수행할 수도 있음
        return -1;
    }
    return sent;
}

// MAP_TCP 클래스에 추가 혹은 수정
int MAP_TCP::sendPacket(const char* data, int len)
{
    if (m_socket == INVALID_SOCKET)
    {
        return -1;
    }

    // 1. 서버가 기대하는 전체 패킷 버퍼 생성
    // 구조: [Header(1)] + [Length(4)] + [Payload(len)]
    std::vector<uint8_t> packet;

    // 2. 헤더 추가 (서버 코드는 현재 0x51만 받도록 되어 있음. 확인 필요!)
    // 주의: 서버 코드가 if (recvBuffer[0] != 0x51)로 되어 있어서 0x51을 보내야 안 짤림.
    // 하지만 논리적으로는 Request면 0x01이어야 함. 
    // 일단 서버가 죽지 않게 하려면 0x51을 넣거나, 서버 코드를 고쳐야 함.
    packet.push_back(0x51);

    // 3. 길이 추가 (4바이트, 리틀 엔디안)
    // C# BitConverter는 리틀 엔디안이므로 htonl() 쓰지 말고 그대로 넣음 (Intel CPU 기준)
    packet.push_back(static_cast<uint8_t>(len & 0xFF));
    packet.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
    packet.push_back(static_cast<uint8_t>((len >> 16) & 0xFF));
    packet.push_back(static_cast<uint8_t>((len >> 24) & 0xFF));

    // 4. 실제 구조체 데이터 추가
    const uint8_t* byteData = reinterpret_cast<const uint8_t*>(data);
    packet.insert(packet.end(), byteData, byteData + len);

    // 5. 전송
    int sent = ::send(m_socket, reinterpret_cast<const char*>(packet.data()), packet.size(), 0);

    if (sent == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        std::cerr << "[MAP_TCP] 전송 실패: " << errorCode << std::endl;
        return -1;
    }
    return sent;
}