// #include "MessageParser.h"
// #include "SystemStatus.h"
// #include <iostream>
// #include <cstring>
// #include <vector>
// #include <iomanip>
// #include <arpa/inet.h>

// namespace {

// using namespace Common;

// unsigned long be64toh(const uint8_t* data) {
//     unsigned long val = 0;
//     for (int i = 0; i < 8; ++i)
//         val = (val << 8) | data[i];
//     return val;
// }

// // 4바이트 빅엔디안 → uint32_t
// uint32_t be32toh(const uint8_t* data) {
//     uint32_t val;
//     std::memcpy(&val, data, 4);
//     return ntohl(val);
// }

// CommonMessage parsePositionRequest(const std::vector<uint8_t>& data, SenderType sender) {
//     CommonMessage msg;

//     // 최소 1바이트(commandType) 확인
//     if (data.size() < 1) {
//         msg.ok = false;
//         return msg;
//     }

//     msg.sender = sender;
//     msg.commandType = CommandType::POSITION_REQUEST_MFR_TO_LC;
//     msg.payload = std::monostate{};  // payload 없음
//     msg.ok = true;

//     return msg;
// }

// CommonMessage parseRadarCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     RadarModeCommand rc;

//     if (data.size() < 6) { // 최소한 commandType + radarId(4) + radarMode
//         msg.ok = false;
//         return msg;
//     }

//     std::memcpy(&rc.radarId, &data[1], 4); // [1~4]
//     rc.radarMode = data[5];                // [5]

//     size_t expectedSize = 6;

//     if (rc.radarMode == 0) { // STOP 모드
//         if (data.size() < 7) { // flag 있어야 함
//             msg.ok = false;
//             return msg;
//         }
//         rc.flag = data[6];    // [6]
//         expectedSize = 7;

//         if (rc.flag == 0x02) {
//             if (data.size() < 11) { // targetId까지 있어야 함
//                 msg.ok = false;
//                 return msg;
//             }
//             std::memcpy(&rc.targetId, &data[7], 4); // [7~10]
//             expectedSize = 11;
//         }
//     }

//     msg.payload = rc;
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }

// CommonMessage parseLSStatus(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     // if (data.size() < 34) {  // 1 (cmd) + 33
//     //     msg.ok = false;
//     //     return msg;
//     // }

//     LSReport ls;
//     ls.lsId  = be32toh(&data[1]);
//     ls.posX  = be64toh(&data[5]);
//     ls.posY  = be64toh(&data[13]);
//     unsigned long angleBits = be64toh(&data[21]);
//     std::memcpy(&ls.angle, &angleBits, sizeof(double));
//     ls.speed = be32toh(&data[29]);
//     ls.mode  = static_cast<OperationMode>(data[33]);

//     // LSReport 디버깅
//     // {
//     //     std::cout << "LSReport 디버깅:" << std::endl;
//     //     std::cout << "  lsId  : " << ls.lsId << std::endl;
//     //     std::cout << "  posX  : " << std::hex << ls.posX << std::endl;
//     //     std::cout << "  posY  : " << std::hex << ls.posY << std::endl;
//     //     std::cout << "  angle : " << std::dec << ls.angle << std::endl;
//     //     std::cout << "  speed : " << std::dec << ls.speed << std::endl; // 10진수로 출력
//     //     std::cout << "  mode  : " << static_cast<int>(ls.mode) << std::endl;                
//     // }

//     msg.payload = ls;
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }


// CommonMessage parseLauncherCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 6) { // cmd(1) + lsId(4) + lsMode(1)
//         msg.ok = false;
//         return msg;
//     }

//     Common::LauncherCommand lc;
//     msg.commandType = static_cast<CommandType>(data[0]);  

//     std::memcpy(&lc.lsId, &data[1], 4);     // 4바이트 launcherId
//     lc.lsMode = data[5];                   // 1바이트 mode
//     msg.payload = lc;
//     msg.ok = true;
//     return msg;
// }

// CommonMessage parseFireCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 6) {
//         msg.ok = false;
//         return msg;
//     }
//     FireCommand fc;
//     fc.lsId = data[1];
//     std::memcpy(&fc.targetId, &data[2], 4);
//     msg.payload = fc;
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }
// //0x05 -> 0x31
// CommonMessage parseMoveCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 18) {
//         msg.ok = false;
//         return msg;
//     }

//     MoveCommand mv;
//     std::memcpy(&mv.lsId, &data[1], 4);         // ✅ 4바이트 복사
//     std::memcpy(&mv.posX, &data[5], 8);         // ✅ X 위치
//     std::memcpy(&mv.posY, &data[13], 8);        // ✅ Y 위치

//     msg.payload = mv;
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }
// CommonMessage parseRadarStatus(const std::vector<uint8_t>& data, SenderType sender) {
//     CommonMessage msg;
//     if (data.size() < 29) { // 4+16+1+8+1+1 = 31 → 최소 29는 필요
//         msg.ok = false;
//         return msg;
//     }

//     msg.sender = sender;
//     msg.commandType = CommandType::STATUS_RESPONSE_MFR_TO_LC;

//     RadarStatus rs;
//     std::memcpy(&rs.radarId,       &data[1], 4);
//     std::memcpy(&rs.posX,          &data[5], 8);
//     std::memcpy(&rs.posY,          &data[13], 8);
//     rs.radarMode     = data[21];
//     std::memcpy(&rs.radarAngle,    &data[22], 8);
//     std::memcpy(&rs.mockTargetId,  &data[30], 4);
//     std::memcpy(&rs.mockMissileId, &data[34], 4);

//     msg.payload = rs;
//     msg.ok = true;
//     return msg;
// }

// //0x21
// CommonMessage parseRadarDetection(const std::vector<uint8_t>& data, SenderType sender) {
//     CommonMessage msg;
//     msg.sender = sender;
//     msg.commandType = CommandType::DETECTION_MFR_TO_LC;
//     msg.ok = true;

//     RadarDetection det;
//     det.radarId = data[1];
//     uint8_t numTargets = data[2];
//     uint8_t numMissiles = data[3];
//     size_t offset = 4;

//     for (int i = 0; i < numTargets && offset + 50 <= data.size(); ++i) {
//         RadarDetection::Target t;
//         std::memcpy(&t.id,         &data[offset],      4);
//         std::memcpy(&t.posX,       &data[offset + 4],  8);
//         std::memcpy(&t.posY,       &data[offset + 12], 8);
//         std::memcpy(&t.altitude,   &data[offset + 20], 8);
//         std::memcpy(&t.speed,      &data[offset + 28], 4);
//         std::memcpy(&t.angle,      &data[offset + 32], 8);
//         std::memcpy(&t.detectTime, &data[offset + 40], 8);
//         t.priority = data[offset + 48];
//         t.hit = data[offset + 49];
//         det.targets.push_back(t);
//         offset += 50;
//     }

//     for (int i = 0; i < numMissiles && offset + 50 <= data.size(); ++i) {
//         RadarDetection::Missile m;
//         std::memcpy(&m.id,           &data[offset],      4);
//         std::memcpy(&m.posX,         &data[offset + 4],  8);
//         std::memcpy(&m.posY,         &data[offset + 12], 8);
//         std::memcpy(&m.altitude,     &data[offset + 20], 8);
//         std::memcpy(&m.speed,        &data[offset + 28], 4);
//         std::memcpy(&m.angle,        &data[offset + 32], 8);
//         std::memcpy(&m.detectTime,   &data[offset + 40], 8);
//         std::memcpy(&m.interceptTime,   &data[offset + 48], 8);
//         m.hit = data[offset + 56];
//         det.missiles.push_back(m);
//         offset += 57;
//     }

//     // 메시지/결과 플래그 파싱 생략 (향후 확장용 필드로 구조체에만 존재)
//     msg.payload = det;
//     return msg;
// }

// } // anonymous namespace

// namespace Common {

// CommonMessage MessageParser::parse(const std::vector<uint8_t>& data, SenderType sender) {
//     CommonMessage msg;
//     msg.sender = sender;

//     if (data.empty()) {
//         std::cerr << "[Parser] 데이터가 비어 있음\n";
//         msg.ok = false;
//         return msg;
//     }

//     // debug
//     /*
//     for (size_t i = 0; i < data.size(); ++i) {
//         std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
//     }
//     */

//     uint8_t cmd = data[0];

//     if (sender == SenderType::ECC) {
//         msg.commandType = static_cast<CommandType>(cmd);

//         switch (msg.commandType) {
//         case CommandType::STATUS_REQUEST_ECC_TO_LC:
//             msg.ok = true;
//             break;
//         case CommandType::SET_RADAR_MODE_ECC_TO_LC:
//             return parseRadarCommand(data, msg);
//         case CommandType::SET_LAUNCHER_MODE_ECC_TO_LC:
//             return parseLauncherCommand(data, msg);
//         case CommandType::FIRE_COMMAND_ECC_TO_LC:
//             return parseFireCommand(data, msg);
//         case CommandType::MOVE_COMMAND_ECC_TO_LC:
//             return parseMoveCommand(data, msg);
//         default:
//             std::cerr << "[Parser] 알 수 없는 ECC commandType: " << static_cast<int>(cmd) << "\n";
//             msg.ok = false;
//             break;
//         }
//     }

//     if (sender == SenderType::MFR) {
//         msg.commandType = static_cast<CommandType>(cmd);

//         switch (msg.commandType) {
//             //0x21
//         case CommandType::STATUS_RESPONSE_MFR_TO_LC: {
//             // data를 hex로 출력
//             std::cout << "[STATUS_RESPONSE_MFR_TO_LC] data (hex): ";
//             for (size_t i = 0; i < data.size(); ++i) {
//                 std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
//             }
//             std::cout << std::dec << std::endl;
//             return parseRadarStatus(data, sender);
//         }
//             //0x22
//         case CommandType::DETECTION_MFR_TO_LC:
//             return parseRadarDetection(data, sender);
//             //0x23
//         case CommandType::POSITION_REQUEST_MFR_TO_LC:
//             return parsePositionRequest(data, sender);
//         default:
//             std::cerr << "[Parser] Unknown MFR commandType: " << static_cast<int>(cmd) << "\n";
//             msg.ok = false;
//             return msg;
//         }
//     }


//     //0x41
//     if (sender == SenderType::LS) {
//         msg.commandType = static_cast<CommandType>(cmd);

//         switch (msg.commandType) {
//         case CommandType::LS_STATUS_UPDATE_LS_TO_LC: {
//             return parseLSStatus(data, msg);  // 예: LS가 상태 갱신 전송
//         }
//         // case CommandType::LS_STATUS_RESPONSE_LS_TO_LC:
//         //     return parseLSStatus(data, msg);  // 예: LS가 이동 완료 등 응답
//         default:
//             std::cerr << "[Parser] 알 수 없는 LS → LC commandType: "
//                     << static_cast<int>(cmd) << "\n";
//             msg.ok = false;
//             break;
//         }
//     }

//     return msg;
// }
// //0x23 


// CommonMessage parseLaunchCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 1 + 4 + 8 + 4 + 8) {  // cmd + launcherId(4) + angle(8) + speed(4) + altitude(8)
//         msg.ok = false;
//         return msg;
//     }
//     LaunchCommand lc;
//     std::memcpy(&lc.launcherId, &data[1], 4);
//     std::memcpy(&lc.launchAngle, &data[5], 8);
//     std::memcpy(&lc.speed, &data[13], 4);
//     std::memcpy(&lc.altitude, &data[17], 8);

//     msg.payload = lc;
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }

// CommonMessage parseMoveCommandLS(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 1 + 4 + 8 + 8) {
//         msg.ok = false;
//         return msg;
//     }
//     MoveCommand mv;
//     std::memcpy(&mv.lsId, &data[1], 4);
//     std::memcpy(&mv.posX, &data[5], 8);
//     std::memcpy(&mv.posY, &data[13], 8);
//     msg.payload = mv;
//     msg.commandType = static_cast<CommandType>(data[0]);

//     msg.ok = true;
//     return msg;
// }

// CommonMessage parseModeChangeCommand(const std::vector<uint8_t>& data, CommonMessage& msg) {
//     if (data.size() < 1 + 4 + 1) {
//         msg.ok = false;
//         return msg;
//     }
//     LauncherModeCommand mc;
//     std::memcpy(&mc.launcherId, &data[1], 4);
//     mc.newMode = static_cast<OperationMode>(data[5]);

//     msg.payload = mc;  // ✅ 이것도 variant에 포함된 타입이어야 함
//     msg.commandType = static_cast<CommandType>(data[0]);
//     msg.ok = true;
//     return msg;
// }

// } // namespace Common