#include "pch.h"

#include "MAP_TCP.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

MAP_TCP::MAP_TCP() : m_socket(INVALID_SOCKET) {
  // Winsock 초기화
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup 실패" << std::endl;
  }
}

MAP_TCP::~MAP_TCP() {
  disconnect();
  WSACleanup();
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
  if (m_socket == INVALID_SOCKET) {
    return -1;
  }
  int sent = ::send(m_socket, data, len, 0);
  if (sent == SOCKET_ERROR) {
    std::cerr << "데이터 전송 실패" << std::endl;
    return -1;
  }
  return sent;
}