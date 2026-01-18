#pragma once
#include <string>

class MAP_TCP {
 public:
  MAP_TCP();
  ~MAP_TCP();

  // IP와 포트를 받아 서버에 연결 시도, 성공 시 true 반환
  bool connect(const std::string& ip, uint16_t port);

  // 연결된 소켓 디스크립터 반환 (연결되지 않았으면 -1)
  int getSocket() const;

  // 연결 종료
  void disconnect();

  // 데이터 전송 함수 추가
  // 성공적으로 전송한 바이트 수 반환, 실패 시 -1 반환
  int send(const char* data, int len);
  int MAP_TCP::sendPacket(const char* data, int len);

 private:
  int m_socket;
};