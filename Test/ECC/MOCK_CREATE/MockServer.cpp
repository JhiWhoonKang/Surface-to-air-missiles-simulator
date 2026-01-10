#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "SendMessageTypes.h"
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define PORT 9000

std::string ReadIni(const std::string& section, const std::string& key,
                    const std::string& path) {
  char buffer[256] = {};
  GetPrivateProfileStringA(section.c_str(), key.c_str(), "", buffer,
                           sizeof(buffer), path.c_str());
  return std::string(buffer);
}

int ReadInt(const std::string& section, const std::string& key,
            const std::string& path) {
  return GetPrivateProfileIntA(section.c_str(), key.c_str(), 0, path.c_str());
}

double ReadDouble(const std::string& section, const std::string& key,
                  const std::string& path) {
  std::string str = ReadIni(section, key, path);
  return str.empty() ? 0.0 : atof(str.c_str());
}
long long ReadLongLong(const std::string& section, const std::string& key,
                       const std::string& path) {
  std::string str = ReadIni(section, key, path);
  return str.empty() ? 0LL : std::stoll(str);
}
void SendSystemStatus(SOCKET client, const std::string& iniPath) {
  // 1. INI 읽기
  RadarStatus radar;
  radar.id = ReadInt("MFR", "mfrId", iniPath);
  radar.position.x = ReadLongLong("MFR", "posX", iniPath);
  radar.position.y = ReadLongLong("MFR", "posY", iniPath);
  radar.mode = static_cast<uint8_t>(ReadInt("MFR", "mode", iniPath));
  radar.angle = ReadDouble("MFR", "degree", iniPath);
  std::cout << "[DEBUG][MFR] mfrId=" << radar.id
            << ", mode=" << static_cast<int>(radar.mode)
            << ", degree=" << radar.angle << ", posX=" << radar.position.x
            << ", posY=" << radar.position.y << std::endl;

  LSStatus ls;
  ls.id = ReadInt("LS", "launchSystemId", iniPath);
  ls.mode = static_cast<uint8_t>(ReadInt("LS", "mode", iniPath));
  ls.angle = ReadDouble("LS", "launchAngle", iniPath);
  ls.position.x = ReadLongLong("LS", "posX", iniPath);
  ls.position.y = ReadLongLong("LS", "posY", iniPath);
  std::cout << "[DEBUG][LS] launchSystemId=" << ls.id
            << ", mode=" << static_cast<int>(ls.mode)
            << ", launchAngle=" << ls.angle << ", posX=" << ls.position.x
            << ", posY=" << ls.position.y << std::endl;

  LCStatus lc;
  lc.id = ReadInt("LC", "commandReady", iniPath);
  lc.position.x = ReadLongLong("LC", "posX", iniPath);
  lc.position.y = ReadLongLong("LC", "posY", iniPath);
  std::cout << "[DEBUG][LC] LCId=" << lc.id << ", posX=" << lc.position.x
            << ", posY=" << lc.position.y << std::endl;

  MissileStatus missile;
  missile.id = ReadInt("Missile", "missileId", iniPath);
  missile.position.x = ReadLongLong("Missile", "posX", iniPath);
  missile.position.y = ReadLongLong("Missile", "posY", iniPath);
  missile.height = ReadInt("Missile", "height", iniPath);
  missile.speed = ReadInt("Missile", "speed", iniPath);
  missile.angle = ReadDouble("Missile", "degree", iniPath);
  missile.predicted_time = ReadInt("Missile", "predicted_time", iniPath);
  missile.intercept_time = ReadInt("Missile", "intercept_time", iniPath);
  missile.hit = static_cast<uint8_t>(ReadInt("Missile", "hit", iniPath));
  std::cout << "[DEBUG][Missile] id=" << missile.id
            << ", posX=" << missile.position.x
            << ", posY=" << missile.position.y << ", height=" << missile.height
            << ", speed=" << missile.speed << ", angle=" << missile.angle
            << ", predicted_time=" << missile.predicted_time
            << ", intercept_time=" << missile.intercept_time
            << ", hit=" << static_cast<int>(missile.hit) << std::endl;

  std::vector<TargetStatus> targets;
  for (int i = 0; i < 100; ++i) {
    std::string section = "Target_" + std::to_string(i);
    unsigned int rawId = ReadInt(section, "targetId", iniPath);
    if (rawId == 0) break;

    TargetStatus t{};
    t.id = rawId;
    t.position.x = ReadLongLong(section, "posX", iniPath);
    t.position.y = ReadLongLong(section, "posY", iniPath);
    t.height = ReadInt(section, "height", iniPath);
    t.speed = ReadInt(section, "speed", iniPath);
    t.angle = ReadDouble(section, "degree", iniPath);
    t.first_detect_time = ReadInt(section, "first_detect_time", iniPath);
    t.priority = static_cast<uint8_t>(ReadInt(section, "priority", iniPath));
    t.hit = static_cast<uint8_t>(ReadInt(section, "hit", iniPath));

    std::cout << "[DEBUG][Target_" << i << "] id=" << t.id
              << ", posX=" << t.position.x << ", posY=" << t.position.y
              << ", height=" << t.height << ", speed=" << t.speed
              << ", angle=" << t.angle
              << ", first_detect_time=" << t.first_detect_time
              << ", priority=" << static_cast<int>(t.priority)
              << ", hit=" << static_cast<int>(t.hit) << std::endl;

    targets.push_back(t);
  }

  // 2. 헤더 구성
  StatusHeader header;
  header.commandType = CommandType::STATUS_RESPONSE;
  header.num_radar = 1;
  header.num_lc = 1;
  header.num_ls = 1;
  header.num_target = (uint8_t)targets.size();
  header.num_missile = 1;

  // 3. 버퍼 구성
  std::vector<uint8_t> buffer;
  auto append = [&](const void* data, size_t size) {
    buffer.insert(buffer.end(), (const uint8_t*)data,
                  (const uint8_t*)data + size);
  };

  append(&header, sizeof(header));
  append(&radar, sizeof(radar));
  append(&ls, sizeof(ls));
  append(&lc, sizeof(lc));
  append(&missile, sizeof(missile));

  for (const auto& t : targets) {
    append(&t, sizeof(t));
  }

  // 4. 고정 크기 3072 바이트 패딩
  // if (buffer.size() < 3072) {
  //    buffer.resize(3072, 0);  // 0으로 채움
  //}

  // 5. 전송
  send(client, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
  std::cout << "[DEBUG] sizeof(RadarStatus) = " << sizeof(RadarStatus) << "\n";
  std::cout << "[DEBUG] sizeof(LCStatus) = " << sizeof(LCStatus) << "\n";
  std::cout << "[DEBUG] sizeof(LSStatus) = " << sizeof(LSStatus) << "\n";
  std::cout << "[DEBUG] sizeof(MissileStatus) = " << sizeof(MissileStatus)
            << "\n";
  std::cout << "[DEBUG] sizeof(TargetStatus) = " << sizeof(TargetStatus)
            << "\n";
  std::cout << "[MockServer] Send Complete (" << buffer.size() << " bytes, "
            << targets.size() << " targets)\n";
}




int main() {
  std::string iniPath =
      "C:\\Users\\user\\Desktop\\Surface-to-air-missiles\\Test\\ECC\\MOCK_"
      "CREATE\\config.ini";

  WSADATA wsaData;
  SOCKET sock;
  sockaddr_in serverAddr;
  const char* serverIP = "127.0.0.1";  // 서버 IP (로컬호스트 예제)
  int serverPort = 9001;               // 서버 포트

  // Winsock 초기화
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed!" << std::endl;
    return 1;
  }

  // 소켓 생성
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET) {
    std::cerr << "Socket creation failed!" << std::endl;
    WSACleanup();
    return 1;
  }

  // 서버 주소 설정
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);
  InetPtonA(AF_INET, serverIP, &serverAddr.sin_addr);

  // 서버에 연결
  if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) ==
      SOCKET_ERROR) {
    std::cerr << "Connection failed!" << std::endl;
    closesocket(sock);
    WSACleanup();
    return 1;
  }

  while (true) {
    char buffer[1];
    SendSystemStatus(sock, iniPath);
    Sleep(1000);

    // int len = recv(sock, buffer, sizeof(buffer), 0);
    //   if (len <= 0) break;
    //   if ((uint8_t)buffer[0] == (uint8_t)CommandType::STATUS_REQUEST) {
    //   }
  }

  closesocket(sock);
  WSACleanup();
  return 0;
}
