// LCManager.cpp
#include <iostream>
#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <chrono>
#include <iomanip>
#include "LCManager.h"
#include "SystemStatus.h"
#include "shared_mutex"
#include "INIReader.h"
#include "Serializer.h"
#include "SerialLS.h"
#include "TcpECC.h" // TcpECC 포함 필요
#include "LCCommandHandler.h"
#include "LCConfig.h"

void LCManager::run()
{
    std::cout << "[LCManager::run] 디버그 루프 실행 준비\n";
    this->startStatusPrintingLoop(); // 🔥 꼭 여기에 추가해야 함

    // std::thread radarLoop([this]() {
    //     // this->startRadarDebugLoop();
    // });
    // radarLoop.detach();

    // std::thread lsLoop([this]() {
    //     // this->startLSCommandLoop();
    // });
    // lsLoop.detach();

    status.lc.LCId = 103001;
    status.lc.position.x = 367080635;
    status.lc.position.y = 1290083312;

    // 상태 주기적으로 받아오기
    std::thread statusLoop([this]()
                           {
        // mfr status get
        while (true) {
        {
            std::vector<uint8_t> packet;
            packet.push_back(0x11);          // commandType
            packet.push_back(0x00);
            packet.push_back(0x00);
            packet.push_back(0x00);
            packet.push_back(0x01);
            mfrSender->sendRaw(packet); // status 명령
        }

        {
            std::vector<uint8_t> packet;
            packet.push_back(0x34);          // commandType
            packet.push_back(0x00);
            packet.push_back(0x00);
            packet.push_back(0x00);
            packet.push_back(0x01);
            serialLS->sendRaw(packet);
        }

        sleep(1);
    } });
    statusLoop.detach();
    // TcpMFR 상태
    // SerialLS 상태
}
// for test
//  void LCManager::startRadarDebugLoop() {
//      while (true) {
//          std::cout << "\n[DEBUG] 입력하세요: 1=상태요청, 2=모드변경(STOP), 0=종료 >> ";
//          int cmd;
//          std::cin >> cmd;

//         std::vector<uint8_t> packet;

//         if (cmd == 1) {
//             packet.push_back(0x11);          // commandType
//             packet.push_back(0x01);          // radarId
//             packet.resize(8, 0x00);          // 8바이트로 고정

//             if (mfrSender) {
//                 mfrSender->sendRaw(packet);
//                 std::cout << "[DEBUG] 상태요청(0x11) 전송 완료 (8 bytes)\n";
//             } else {
//                 std::cerr << "[DEBUG] mfrSender가 설정되지 않음\n";
//             }

//         } else if (cmd == 2) {
//             packet.push_back(0x12);          // commandType
//             packet.push_back(0x01);          // radarId
//             packet.push_back(0x01);          // mode = STOP
//             packet.push_back(0x00);          // targetId (low byte)
//             packet.push_back(0x00);
//             packet.push_back(0x00);
//             packet.push_back(0x00);          // targetId (high byte)
//             packet.push_back(0x00);          // padding or reserved

//             if (mfrSender) {
//                 mfrSender->sendRaw(packet);
//                 std::cout << "[DEBUG] 모드변경(0x12) 전송 완료 (8 bytes)\n";
//             } else {
//                 std::cerr << "[DEBUG] mfrSender가 설정되지 않음\n";
//             }

//         } else if (cmd == 0) {
//             std::cout << "[DEBUG] 디버그 루프 종료\n";
//             break;

//         } else {
//             std::cout << "[DEBUG] 잘못된 명령\n";
//         }
//     }
// }

SystemStatus LCManager::getStatusCopy() const
{
    std::lock_guard<std::mutex> lock(statusMutex);
    return status;
}

void LCManager::withLockedStatus(std::function<void(SystemStatus &)> func)
{
    std::lock_guard<std::mutex> lock(statusMutex);
    func(status);
}

// UpdateStatus overloads
void LCManager::updateStatus(const MFRStatus &mfr)
{
    withLockedStatus([&](SystemStatus &s)
                     { s.mfr = mfr; });
}

void LCManager::updateStatus(const LSStatus &ls)
{
    withLockedStatus([&](SystemStatus &s)
                     { s.ls = ls; });
}

void LCManager::updateStatus(const LCStatus &lc)
{
    withLockedStatus([&](SystemStatus &s)
                     { s.lc = lc; });
}

void LCManager::updateStatus(const std::vector<MissileStatus> &missiles)
{
    withLockedStatus([&](SystemStatus &s)
                     { s.missiles = missiles; });
}

void LCManager::updateStatus(const std::vector<TargetStatus> &targets)
{
    withLockedStatus([&](SystemStatus &s)
                     { s.targets = targets; });
}

void LCManager::deleteTargetById(unsigned int targetId)
{
    withLockedStatus([&](SystemStatus &s)
                     {
        auto &targets = s.targets;
        targets.erase(std::remove_if(targets.begin(), targets.end(),
                                      [targetId](const TargetStatus &t) { return t.id == targetId; }),
                      targets.end()); });
}

void LCManager::deleteMissileById(unsigned int missileId)
{
    withLockedStatus([&](SystemStatus &s)
                     {
        auto &missiles = s.missiles;
        missiles.erase(std::remove_if(missiles.begin(), missiles.end(),
                                       [missileId](const MissileStatus &m) { return m.id == missileId; }),
                       missiles.end()); });
}

void LCManager::onMessage(const Common::CommonMessage &msg)
{
    dispatch(msg);
}

void LCManager::dispatch(const Common::CommonMessage &msg)
{
    using namespace Common;
    static int dispatchCounter = 0;
    dispatchCounter++;

    if (dispatchCounter % 10 == 0)
    {
        // std::cout << "[dispatch] 수신됨: Sender=" << static_cast<int>(msg.sender)
        //   << ", Command=" << static_cast<int>(msg.commandType)
        //   << ", OK=" << msg.ok << "\n";
    }

    // if (!msg.ok) {
    //     std::cerr << "[dispatch] ⚠️ 파싱 실패 or 포맷 이상\n";
    //     return;
    // }

    switch (msg.sender)
    {
    case SenderType::ECC:
        LCCommandHandler::handleECCCommand(msg, *this);
        break;
    case SenderType::MFR:
        LCCommandHandler::handleMFRCommand(msg, *this);
        break;
    case SenderType::LS:
        LCCommandHandler::handleLSCommand(msg, *this);
        break;
    default:
        // std::cerr << "[dispatch] ❌ 알 수 없는 송신자(SenderType=" << static_cast<int>(msg.sender) << ") 무시됨\n";
        break;
    }
}

double LCManager::LaunchAngleCalc()
{
    SystemStatus snapshot = getStatusCopy();
    if (snapshot.targets.empty())
        return -1;

    const Pos2D &lsPos = snapshot.ls.position;

    // posX/posY 직접 사용
    double dx = static_cast<double>(snapshot.targets[0].posX - lsPos.x);
    double dy = static_cast<double>(snapshot.targets[0].posY - lsPos.y);
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    std::cout << "[LaunchAngleCalc] θ: " << angle << "도\n";
    return angle;
}

double LCManager::DetectionAngleCalc()
{
    SystemStatus snapshot = getStatusCopy();

    if (snapshot.targets.empty())
        return -1;

    const Pos2D &radarPos = snapshot.mfr.position;

    // posX, posY 직접 접근
    double dx = static_cast<double>(snapshot.targets[0].posX - radarPos.x);
    double dy = static_cast<double>(snapshot.targets[0].posY - radarPos.y);
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    std::cout << "[DetectionAngleCalc] θ: " << angle << "도\n";
    return angle;
}

void LCManager::sendStatus()
{
    static int sendCounter = 0;
    sendCounter++;

    SystemStatus snapshot = getStatusCopy(); // 상태 스냅샷

    if (sendCounter % 10 == 0)
    {

        // std::cout << "[LCManager] 상태를 ECC로 전송 중...\n";
        std::cout << std::dec;

        // // MFR 상태 출력
        // std::cout << "- [MFR] ID: " << snapshot.mfr.mfrId
        //           << ", Mode: " << static_cast<int>(snapshot.mfr.mode)
        //           << ", Degree: " << snapshot.mfr.degree
        //           << ", Detect Time : " << snapshot.mfr.detectTime
        //           << ", Pos: (" << snapshot.mfr.position.x << ", " << snapshot.mfr.position.y
        //           << ", " << snapshot.mfr.height << ")\n";

        // // LS 상태 출력
        // std::cout << "- [LS] ID: " << snapshot.ls.launchSystemId
        //           << ", Mode: " << static_cast<int>(snapshot.ls.mode)
        //           << ", Angle: " << snapshot.ls.launchAngle
        //           << ", Pos: (" << snapshot.ls.position.x << ", " << snapshot.ls.position.y
        //           << ", " << snapshot.ls.height << ")\n";

        // // LC 상태 출력
        // std::cout << "- [LC] ID: " << snapshot.lc.LCId
        //           << ", Pos: (" << snapshot.lc.position.x << ", " << snapshot.lc.position.y
        //           << ", " << snapshot.lc.height << ")\n";

        // 타겟 정보출력
        // for (const auto target : snapshot.targets)
        // {
        //     std::cout << "- [Target] ID: " << target.id
        //               << ", Pos: (" << target.posX << ", " << target.posY
        //               << "), Altitude: " << target.altitude
        //               << ", Speed: " << target.speed
        //               << ", Angle1: " << target.angle1
        //               << ", Angle2: " << target.angle2
        //               << ", Detect Time: " << target.detectTime
        //               << ", Priority: " << static_cast<int>(target.priority)
        //               << ", Hit: " << (target.hit ? "Yes" : "No") << "\n";
        // }
    }

    // 직렬화 및 전송
    std::vector<uint8_t> packet = Common::Serializer::serializeStatusResponse(snapshot);
    for (const auto &m : snapshot.missiles)
    {
        if (m.hit == true)
        {
            deleteTargetById(m.id);
            std::cout << "[LCManager] 미사일 ID " << m.id << "가 파괴되어 삭제되었습니다.\n";
        }
    }

    for (const auto &t : snapshot.targets)
    {
        if (t.hit == true)
        {
            deleteTargetById(t.id);
            std::cout << "[LCManager] 타겟 ID " << t.id << "가 파괴되어 삭제되었습니다.\n";
        }
    }

    if (consoleSender)
    {
        consoleSender->sendRaw(packet);

        // if (sendCounter % 10 == 0) {
        //     std::cout << "[LCManager] ECC로 상태 메시지 전송 완료 (" << static_cast<int>(packet.size()) << " 바이트)\n";
        // }
    }
    else
    {
        std::cerr << "[LCManager] consoleSender가 연결되지 않았습니다.\n";
    }
}

void LCManager::setTargetLock(unsigned int targetId)
{
    locked_target_id = targetId;
}

void LCManager::getTargetLock(unsigned int &targetId) const
{
    targetId = locked_target_id;
}

void LCManager::initialize(const std::string &iniPath)
{
    INIReader reader(iniPath);
    if (reader.ParseError() < 0)
    {
        std::cerr << "[ERROR] 설정 파일 읽기 실패: " << iniPath << "\n";
        return;
    }

    withLockedStatus([&](SystemStatus &s)
                     {
        // [MFR]
        s.mfr.mfrId = reader.GetInteger("MFR", "mfrId", 0);
        s.mfr.mode = static_cast<MFRMode>(reader.GetInteger("MFR", "mode", 0));
        s.mfr.degree = reader.GetReal("MFR", "degree", 0.0);
        s.mfr.position.x = reader.GetLongLong("MFR", "posX", 0);
        s.mfr.position.y = reader.GetLongLong("MFR", "posY", 0);
        std::cout << "[DEBUG][MFR] mfrId=" << s.mfr.mfrId
                  << ", mode=" << static_cast<int>(s.mfr.mode)
                  << ", degree=" << s.mfr.degree
                  << ", posX=" << s.mfr.position.x
                  << ", posY=" << s.mfr.position.y << std::endl;

        // [LS]
        s.ls.launchSystemId = reader.GetInteger("LS", "launchSystemId", 0);
        s.ls.mode = static_cast<LauncherMode>(reader.GetInteger("LS", "mode", 0));
        s.ls.launchAngle = reader.GetReal("LS", "launchAngle", 0.0);
        s.ls.position.x = reader.GetLongLong("LS", "posX", 0);
        s.ls.position.y = reader.GetLongLong("LS", "posY", 0);
        std::cout << "[DEBUG][LS] launchSystemId=" << s.ls.launchSystemId
                  << ", mode=" << static_cast<int>(s.ls.mode)
                  << ", launchAngle=" << s.ls.launchAngle
                  << ", posX=" << s.ls.position.x
                  << ", posY=" << s.ls.position.y << std::endl;

        // [LC]
        s.lc.LCId = reader.GetInteger("LC", "commandReady", 0);
        s.lc.position.x = reader.GetLongLong("LC", "posX", 0);
        s.lc.position.y = reader.GetLongLong("LC", "posY", 0);
        s.lc.calculated_time = getCurrentTimeMillis();
        std::cout << "[DEBUG][LC] LCId=" << s.lc.LCId
                  << ", posX=" << s.lc.position.x
                  << ", posY=" << s.lc.position.y << std::endl;

        // [Missile]
        if (reader.HasSection("Missile")) {
            MissileStatus missile;
            missile.id = reader.GetInteger("Missile", "missileId", 0);
            missile.posX = reader.GetLongLong("Missile", "posX", 0);
            missile.posY = reader.GetLongLong("Missile", "posY", 0);
            missile.altitude = reader.GetLongLong("Missile", "height", 0);
            missile.speed = reader.GetInteger("Missile", "speed", 0);
            missile.angle = reader.GetReal("Missile", "degree", 0.0);
            missile.detectTime = reader.GetLongLong("Missile", "predicted_time", 0);
            missile.interceptTime = reader.GetLongLong("Missile", "intercept_time", 0);
            missile.hit = reader.GetBoolean("Missile", "hit", false);
            s.missiles.push_back(missile);

            std::cout << "[DEBUG][Missile] id=" << missile.id
                      << ", posX=" << missile.posX
                      << ", posY=" << missile.posY
                      << ", altitude=" << missile.altitude
                      << ", speed=" << missile.speed
                      << ", angle=" << missile.angle
                      << ", detectTime=" << missile.detectTime
                      << ", interceptTime=" << missile.interceptTime
                      << ", hit=" << missile.hit << std::endl;
        }

        // [Target_*]
        for (int i = 0; i < 50; ++i) {
            std::string section = "Target_" + std::to_string(i);
            if (!reader.HasSection(section)) break;

            TargetStatus target;
            target.id = reader.GetInteger(section, "targetId", 0);
            target.posX = reader.GetLongLong(section, "posX", 0);
            target.posY = reader.GetLongLong(section, "posY", 0);
            target.altitude = reader.GetLongLong(section, "height", 0);
            target.speed = reader.GetInteger(section, "speed", 0);
            target.angle1 = reader.GetReal(section, "degree1", 0.0);
            target.angle2 = reader.GetReal(section, "degree2", 0.0);
            target.detectTime = reader.GetLongLong(section, "first_detect_time", 0);
            target.priority = static_cast<uint8_t>(reader.GetInteger(section, "priority", 0));
            target.hit = reader.GetBoolean(section, "hit", false);

            s.targets.push_back(target);

            std::cout << "[DEBUG][Target_" << i << "] id=" << target.id
                      << ", posX=" << target.posX
                      << ", posY=" << target.posY
                      << ", altitude=" << target.altitude
                      << ", speed=" << target.speed
                      << ", angle=" << target.angle1
                      << ", angle=" << target.angle2
                      << ", detectTime=" << target.detectTime
                      << ", priority=" << static_cast<int>(target.priority)
                      << ", hit=" << target.hit << std::endl;
        } });
}

void LCManager::init(const std::string &configPath, const std::string &ip, int port)
{
    ConfigCommon config;
    loadConfig("./../Config/LC.ini", config);

    // 설정 파일 초기화 (필요 시 활성화)
    // initialize(configPath);

    // ✅ ECC 연결
    auto ecc = std::make_shared<TcpECC>(config.ECCRecvIP, config.ECCRecvPort);
    ecc->setCallback(this);
    setConsoleSender(ecc);
    ecc->start();

    // ✅ MFR 연결 (ECC와 동일한 구조)
    auto mfr = std::make_shared<TcpMFR>(config.MFRRecvIP, config.MFRRecvPort);
    mfr->setCallback(this);
    setMFRSender(mfr);
    mfr->start();

    // ✅ LS 연결 (Serial UDP 방식)
    serialLS = std::make_shared<SerialLS>(
        /* localPort */ config.LSRecvPort,
        /* lcIp */ config.LSSendIP,
        /* lcPort */ config.LSSendPort);
    serialLS->setCallback(this);
    serialLS->start();
}

long long LCManager::squaredDistance(const Pos2D &a, const Pos2D &b)
{
    long long dx = a.x - b.x;
    long long dy = a.y - b.y;
    return dx * dx + dy * dy;
}

double LCManager::calculateDetectionAngle(const Pos2D &from, const Pos2D &to)
{
    double dx = static_cast<double>(to.x - from.x);
    double dy = static_cast<double>(to.y - from.y);
    return atan2(dy, dx) * 180.0 / M_PI;
}

TargetStatus *LCManager::findTargetById(std::vector<TargetStatus> &targets, unsigned int id)
{
    for (auto &t : targets)
    {
        if (t.id == id)
            return &t;
    }
    return nullptr;
}

void LCManager::updateCalTime(const unsigned long long &calTime)
{
    status.lc.calculated_time = calTime;
}

void LCManager::printStatus(const SystemStatus &status)
{
    std::cout << "[MFR] ID: " << status.mfr.mfrId
              << ", Mode: " << static_cast<unsigned int>(status.mfr.mode)
              << ", Degree: " << status.mfr.degree
              << ", Pos: (" << status.mfr.position.x << ", " << status.mfr.position.y << ")\n";

    std::cout << "[LS] ID: " << status.ls.launchSystemId
              << ", Mode: " << static_cast<unsigned int>(status.ls.mode)
              << ", Angle: " << status.ls.launchAngle
              << ", Pos: (" << status.ls.position.x << ", " << status.ls.position.y << ")\n";

    std::cout << "[Missile List] 총 " << status.missiles.size() << "개\n";
    for (const auto &m : status.missiles)
    {
        std::cout << "  - ID: " << static_cast<int>(m.id)
                  << ", Angle: " << m.angle
                  << ", Pos: (" << m.posX << ", " << m.posY << ")"
                  << ", Hit: " << static_cast<int>(m.hit) << "\n";
    }

    std::cout << "[Target List] 총 " << status.targets.size() << "개\n";
    for (const auto &t : status.targets)
    {
        std::cout << "  - ID: " << t.id
                  << ", Angle: " << t.angle1
                  << ", Angle: " << t.angle2
                  << ", Pos: (" << t.posX << ", " << t.posY << ")"
                  << ", Speed: " << t.speed
                  << ", Priority: " << static_cast<int>(t.priority)
                  << ", Hit: " << static_cast<int>(t.hit) << "\n";
    }
}
std::string LCManager::getRadarCommandInput()
{
    std::string input;
    std::cout << "\n[입력] 레이더 제어 명령 (y=자동고정 / resume=회전모드로 전환 / id=숫자 / 아무 키나=무시): ";
    std::getline(std::cin, input);
    return input;
}

void LCManager::onRadarStatusReceived(const Common::RadarStatus &r)
{
    static int counter = 0;
    counter++;

    // MFR 상태 구성
    MFRStatus m;
    m.mfrId = r.radarId;
    m.mode = static_cast<MFRMode>(r.radarMode);
    m.degree = r.radarAngle;
    m.position.x = r.posX;
    m.position.y = r.posY;
    m.height = r.height; // ✅ 정확한 위치에 할당

    updateStatus(m);

    // 5번마다 출력
    // if (counter % 5 == 0) {
    //     std::cout << "[MFR] 상태 갱신 완료\n";
    //     std::cout << "  - ID       : " << static_cast<unsigned int>(r.radarId) << "\n";
    //     std::cout << "  - 모드     : " << static_cast<unsigned int>(r.radarMode) << "\n";
    //     std::cout << "  - 각도     : " << r.radarAngle << "\n";
    //     std::cout << "  - 위치     : (" << r.posX << ", " << r.posY << ", " << r.height << ")\n";  // ✅ height 출력
    // }
}

void LCManager::onRadarDetectionReceived(const Common::RadarDetection &d)
{
    std::vector<TargetStatus> targets;
    bool lockedTargetFound = false;
    for (const auto &t : d.targets)
    {
        TargetStatus ts;
        ts.id = t.id;
        ts.angle1 = t.angle1;
        ts.angle2 = t.angle2;
        ts.posX = t.posX;
        ts.posY = t.posY;
        ts.altitude = t.altitude;
        ts.speed = t.speed;
        ts.detectTime = t.detectTime;
        ts.priority = t.priority;
        ts.hit = t.hit;
        targets.push_back(ts);
        if (t.id == locked_target_id)
        {
            lockedTargetFound = true; // 현재 잠금된 타겟이 탐지됨
        }
        // std::cout << "[MFR] 타겟 정보: ID=" << ts.id
        //           << ", Pos=(" << ts.posX << ", " << ts.posY << ")"
        //           << ", Altitude=" << ts.altitude
        //           << ", Speed=" << ts.speed
        //           << ", Angle1=" << ts.angle1
        //           << ", Angle2=" << ts.angle2
        //           << ", DetectTime=" << ts.detectTime
        //           << ", Priority=" << static_cast<int>(ts.priority)
        //           << ", Hit=" << static_cast<int>(ts.hit) << "\n";
    }
    /*if (lockedTargetFound == false)
    {
        // 현재 잠금된 타겟이 탐지되지 않은 경우, 잠금 해제
        Common::RadarModeCommand radarCmd;
        std::cout << "[MFR] 현재 잠금된 타겟이 탐지되지 않았습니다. 잠금 해제됨.\n";
        radarCmd.radarId = locked_target_id;
        radarCmd.radarMode = 0x02;       // Rotate
        radarCmd.flag = 0x00;            // 사용 안 할 경우라도 초기화
        radarCmd.priority_select = 0x02; // targetId 있음
        radarCmd.targetId = 0x00;
        auto radarPacket = Common::Serializer::serializeRadarModeChange(radarCmd);
        if (hasMFRSender())
        {
            sendToMFR(radarPacket);
            std::cout << "[LC] 레이더 정지모드 전송 → radarId=" << radarCmd.radarId
                      << ", targetId=" << radarCmd.targetId << "\n";
        }
        locked_target_id = 0;
    }
        */
    updateStatus(targets);
    std::cout << "[MFR] 타겟 정보 갱신 완료 (총 " << targets.size() << "개)\n";

    // 이후에 hit처리 로그 삭제

    std::vector<MissileStatus> missiles;
    for (const auto &m : d.missiles)
    {
        MissileStatus ms;
        ms.id = m.id; // struct에 정의된 필드 사용
        ms.posX = m.posX;
        ms.posY = m.posY;
        ms.altitude = m.altitude;
        ms.speed = m.speed;
        ms.angle = m.angle;
        ms.interceptTime = m.detectTime;
        ms.hit = m.hit;
        missiles.push_back(ms);
    }
    updateStatus(missiles);

    static int detectionCounter = 0;
    detectionCounter++;

    // 이후에 hit처리 로그 삭제
    // 미사일 정보 출력
    // if(detectionCounter%10==0){
    //     std::cout << "[MFR] 미사일 정보 (총 " << missiles.size() << "개)\n";
    //     for (const auto& ms : missiles) {
    //         std::cout << "  - ID: " << static_cast<int>(ms.id)
    //                 << ", Pos: (" << ms.posX << ", " << ms.posY << ")"
    //                 << ", Altitude: " << ms.altitude
    //                 << ", Speed: " << ms.speed
    //                 << ", Angle: " << ms.angle
    //                 << ", InterceptTime: " << ms.interceptTime
    //                 << ", Hit: " << static_cast<int>(ms.hit) << "\n";
    //     }
    // }

    {
        // std::cout << "[MFR] 탐지 정보 갱신 완료 (타겟 " << targets.size()
        //   << ", 미사일 " << missiles.size() << ")\n";
    }
}

void LCManager::setConsoleSender(std::shared_ptr<IStatusSender> sender)
{
    consoleSender = std::move(sender);
}

void LCManager::setMFRSender(std::shared_ptr<IStatusSender> sender)
{
    mfrSender = std::move(sender);
}

void LCManager::sendToLS(const std::vector<uint8_t> &packet)
{
    if (serialLS)
    {
        serialLS->sendRaw(packet);
    }
    else
    {
        std::cerr << "[LCManager] LS 송신자(serialLS)가 설정되지 않았습니다. 전송 실패.\n";
    }
}

// ----------- 외부에서 Sender 있는지 확인 함수 3*2개
bool LCManager::hasConsoleSender() const
{
    return static_cast<bool>(consoleSender);
}

void LCManager::sendToConsole(const std::vector<uint8_t> &packet)
{
    if (consoleSender)
    {
        consoleSender->sendRaw(packet);
    }
    else
    {
        std::cerr << "[LCManager] ECC 송신자(consoleSender)가 설정되지 않았습니다. 전송 실패.\n";
    }
}

bool LCManager::hasMFRSender() const
{
    return static_cast<bool>(mfrSender);
}

void LCManager::sendToMFR(const std::vector<uint8_t> &packet)
{
    if (mfrSender)
    {
        mfrSender->sendRaw(packet);
        std::cout << "[LCManager] MFR로 " << packet.size() << "바이트 전송 완료.\n";
    }
    else
    {
        std::cerr << "[LCManager] MFR 송신자(mfrSender)가 설정되지 않았습니다. 전송 실패.\n";
    }
}

bool LCManager::hasLSSender() const
{
    return static_cast<bool>(serialLS);
}

void LCManager::onLCPositionRequest()
{
    SystemStatus snapshot = getStatusCopy();
    Common::LCPositionResponse res{
        .radarId = snapshot.mfr.mfrId,
        .posX = snapshot.lc.position.x,
        .posY = snapshot.lc.position.y,
        .height = snapshot.lc.height // ✅ height 필드 추가
    };
    auto packet = Common::Serializer::serializeLCPositionResponse(res);
    sendToMFR(packet);

    std::cout << "[LCManager] MFR에 LC 위치 응답 전송 완료 → radarId: "
              << res.radarId << ", Pos: (" << res.posX << ", " << res.posY
              << "), Height: " << res.height << '\n';
}

// 0x33에서 파생됨

void LCManager::onLSStatusReceived(const Common::LSReport &ls)
{
    static int counter = 0;
    counter++;

    LSStatus internalLS;
    internalLS.launchSystemId = ls.lsId;
    internalLS.mode = static_cast<LauncherMode>(ls.mode);
    internalLS.launchAngle = ls.launchAngle;
    internalLS.position.x = ls.posX;
    internalLS.position.y = ls.posY;
    internalLS.height = ls.height;
    internalLS.speed = ls.speed;

    updateStatus(internalLS); // SystemStatus 안의 ls 항목 업데이트

    // if (counter % 5 == 0) {
    //     std::cout << "[LS] 상태 갱신 완료\n";
    //     std::cout << std::dec;  // 출력 10진수로 설정
    //     std::cout << "  - ID       : " << ls.lsId << "\n";
    //     std::cout << "  - 위치     : (" << ls.posX << ", " << ls.posY << ", " << ls.height << ")\n";
    //     std::cout << "  - 각도     : " << ls.launchAngle << "\n";
    //     std::cout << "  - 속도     : " << ls.speed << "\n";
    //     std::cout << "  - 모드     : " << static_cast<int>(ls.mode) << "\n";
    // }
}

// *******************FOR TEST
void LCManager::startLSCommandLoop()
{
    std::cout << "[LS] Serial 포트 수신만 실행 중... 종료하려면 Ctrl+C\n";

    if (serialLS)
    {
        serialLS->start(); // 내부 receiveLoop() 실행됨
    }
    else
    {
        std::cerr << "[LS] serialLS 인스턴스가 설정되지 않았습니다.\n";
    }

    // 아무 일도 하지 않음. 단순히 스레드가 돌아가게 유지
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void LCManager::startStatusPrintingLoop()
{
    std::thread([this]()
                {
        while (true) {
            //this->printStatus();  // 현재 위치 상태 출력
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } })
        .detach();
}

void LCManager::printStatus() const
{
    SystemStatus snapshot = getStatusCopy(); // thread-safe하게 복사
    std::cout << std::dec;
    std::cout << "\n\n";
    std::cout << "----| Launch System (LS)\n";
    std::cout << "  - ID: " << snapshot.ls.launchSystemId << "\n";
    std::cout << "  - Pos: (" << snapshot.ls.position.x << ", " << snapshot.ls.position.y << ")\n";
    std::cout << "  - Altitude: " << snapshot.ls.height << "\n";
    std::cout << "  - Mode: " << static_cast<int>(snapshot.ls.mode) << "\n";
    std::cout << "  - LaunchAngle: " << snapshot.ls.launchAngle << "\n";

    std::cout << std::dec;
    std::cout << "----| Radar (MFR)\n";
    std::cout << "  - ID: " << snapshot.mfr.mfrId << "\n";
    std::cout << "  - Pos: (" << snapshot.mfr.position.x << ", " << snapshot.mfr.position.y << ")\n";
    std::cout << "  - Altitude: " << snapshot.mfr.height << "\n";
    std::cout << "  - Mode: " << static_cast<int>(snapshot.mfr.mode) << "\n";
    std::cout << "  - Degree: " << snapshot.mfr.degree << "\n";

    std::cout << std::dec;
    std::cout << "----| Launcher Controller (LC)\n";
    std::cout << "  - ID: " << snapshot.lc.LCId << "\n";
    std::cout << "  - Pos: (" << snapshot.lc.position.x << ", " << snapshot.lc.position.y << ")\n";
    std::cout << "  - Height: " << snapshot.lc.height << "\n";

    std::cout << "----| Missile List (" << snapshot.missiles.size() << "개)\n";
    std::cout << std::dec;
    for (const auto &m : snapshot.missiles)
    {
        std::cout << "  - ID: " << m.id
                  << ", Pos: (" << m.posX << ", " << m.posY << ")"
                  << ", Altitude: " << m.altitude
                  << ", Speed: " << m.speed
                  << ", Angle: " << m.angle
                  << ", InterceptTime: " << m.interceptTime
                  << ", Hit: " << static_cast<int>(m.hit) << "\n";
    }

    std::cout << std::dec;
    std::cout << "----| Target List (" << snapshot.targets.size() << "개)\n";
    for (const auto &t : snapshot.targets)
    {
        std::cout << "  - ID: " << t.id
                  << ", Pos: (" << t.posX << ", " << t.posY << ")"
                  << ", Altitude: " << t.altitude
                  << ", Speed: " << t.speed
                  << ", Angle1: " << t.angle1
                  << ", Angle2: " << t.angle2
                  << ", Priority: " << static_cast<int>(t.priority)
                  << ", Hit: " << static_cast<int>(t.hit) << "\n";
    }
}
