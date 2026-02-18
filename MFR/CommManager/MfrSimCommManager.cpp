#include "MfrSimCommManager.h"
#include "PacketProtocol.h"
#include "logger.h"
#include "CommonPacket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <iostream>

namespace
{
    constexpr size_t BUFFER_SIZE = 1024;
}

MfrSimCommManager::MfrSimCommManager(std::shared_ptr<IReceiver> receiver)
    : receiver_(std::move(receiver)), sockfd(-1), simPort(0), isRunning_(false)
{
    g_lastSeqID = 0;
    g_totalPackets = 0;
    g_integrityFail = 0;
    g_lossCount = 0;
    initMfrSimCommManager();
}

MfrSimCommManager::~MfrSimCommManager()
{
    stopReceiver();
    if (sockfd >= 0)
    {
        close(sockfd);
        Logger::log("[MfrSimCommManager] Socket closed");
    }
}

void MfrSimCommManager::initMfrSimCommManager()
{
    Logger::log("[MfrSimCommManager] Initializing UDP communication");

    const auto &config = MfrConfig::getInstance();
    simPort = config.simulatorPort;

    Logger::log("[MfrSimCommManager] Initializing with Simulator Port: " + std::to_string(simPort));

    if (connectToSim())
    {
        startUdpReceiver();
    }
}

bool MfrSimCommManager::connectToSim()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        Logger::log("[MfrSimCommManager] Failed to create UDP socket");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(simPort);

    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        Logger::log("[MfrSimCommManager] Failed to bind UDP socket");
        close(sockfd);
        return false;
    }

    Logger::log("[MfrSimCommManager] Successfully connected to simulator");
    return true;
}

void MfrSimCommManager::startUdpReceiver()
{
    if (isRunning_)
    {
        Logger::log("[MfrSimCommManager] Receiver thread already running");
        return;
    }

    isRunning_ = true;
    receiverThread = std::thread(&MfrSimCommManager::runReceiver, this);
}

void MfrSimCommManager::runReceiver()
{
    Logger::log("[MfrSimCommManager] UDP Receiver thread started");

    std::vector<char> buffer(BUFFER_SIZE);
    sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    while (isRunning_)
    {
        // 소켓 타임아웃 설정
        struct timeval tv;
        tv.tv_sec = 1; // 1초 타임아웃
        tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        ssize_t len = recvfrom(sockfd, buffer.data(), buffer.size(), 0,
                               reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);

        if (len <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 타임아웃 - 계속 진행
                continue;
            }
            if (isRunning_)
            {
                Logger::log("[MfrSimCommManager] Error receiving data");
            }
            break;
        }

        processReceivedData(buffer.data(), len);
    }

    Logger::log("[MfrSimCommManager] UDP Receiver thread stopped");
}

void MfrSimCommManager::processReceivedData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock();
    if (!receiver) {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    // 1. [검증 로직] 새로운 프로토콜인지 확인 (헤더 크기 + Magic Number)
    if (len >= sizeof(PacketHeader))
    {
        const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);
        
        // 매직 넘버가 맞으면 -> "검증된 배치 패킷"으로 처리
        if (header->magic == 0xA1B2C3D4) 
        {
            processBatchPacket(buffer, len);
            return; 
        }
    }

    // 2. [기존 로직] 매직 넘버가 없으면 기존 방식대로 처리 (미사일 데이터 등)
    if (len == sizeof(uint8_t) + sizeof(TargetSimData))
    {
        // 구형 단일 표적 패킷 처리 (필요 없다면 삭제 가능)
        processTargetData(buffer, len);
    }
    else if (len == sizeof(uint8_t) + sizeof(MissileSimData))
    {
        processMissileData(buffer, len);
    }
    else
    {
        // Logger::log("[MfrSimCommManager] Unknown packet size: " + std::to_string(len));
    }
}

// [신규 추가] 검증 및 배치 처리 함수
void MfrSimCommManager::processBatchPacket(const char* buffer, size_t len)
{
    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);
    const char* payloadStart = buffer + sizeof(PacketHeader);
    size_t payloadLen = len - sizeof(PacketHeader);

    // ---------------------------------------------------------
    // 1. 무결성 검증 (Integrity Check) - CRC
    // ---------------------------------------------------------
    uint32_t calcCRC = calculateCRC32(payloadStart, payloadLen);
    
    if (calcCRC != header->payloadCRC)
    {
        g_integrityFail++;
        Logger::log("[Integrity Fail] CRC Mismatch! Packet Dropped.");
        return; // 데이터가 깨졌으므로 통째로 버림
    }

    // ---------------------------------------------------------
    // 2. 패킷 손실 확인 (Sequence Check)
    // ---------------------------------------------------------
    if (g_totalPackets > 0)
    {
        if (header->seqID != g_lastSeqID + 1)
        {
            uint32_t lost = header->seqID - g_lastSeqID - 1;
            g_lossCount += lost;
            // Logger::log("[Packet Loss] Missed " + std::to_string(lost) + " packets.");
        }
    }
    g_lastSeqID = header->seqID;
    g_totalPackets++;

    // ---------------------------------------------------------
    // 3. 데이터 파싱 및 상위 레이어 전달
    // ---------------------------------------------------------
    // 검증이 끝났으니 안심하고 데이터를 꺼냅니다.
    const TargetSimData* targets = reinterpret_cast<const TargetSimData*>(payloadStart);

    auto receiver = receiver_.lock();
    if (!receiver) return;

    {
        std::lock_guard<std::mutex> lock(callbackMutex_);

        // 배치로 들어온 표적들을 하나씩 꺼내서 기존 callback 호출
        for (uint32_t i = 0; i < header->count; ++i)
        {
            // 상위 레이어가 [1 byte Type] + [TargetData] 구조를 원한다면
            // 여기서 그 구조를 만들어서 던져줘야 함.
            
            // 임시 버퍼 생성 (기존 호환성 유지용)
            std::vector<char> packet;
            packet.reserve(sizeof(uint8_t) + sizeof(TargetSimData));

            uint8_t type = 1; // 예: Target 데이터 타입 ID (기존 코드의 buffer[0] 값 참조 필요)
            packet.push_back(type);
            
            const char* targetPtr = reinterpret_cast<const char*>(&targets[i]);
            packet.insert(packet.end(), targetPtr, targetPtr + sizeof(TargetSimData));

            // 기존 콜백 호출 (하나씩 전달)
            receiver->callBackData(packet);
        }
    }
}

void MfrSimCommManager::processMissileData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock();
    if (!receiver)
    {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    try
    {
        MissileSimData data;
        std::memcpy(&data, buffer + 1, sizeof(MissileSimData));
        std::vector<char> packet(buffer, buffer + len);
        {
            std::lock_guard<std::mutex> lock(callbackMutex_); // 스레드 안전을 위한 뮤텍스 사용
            receiver->callBackData(packet);
        }
    }
    catch (const std::exception &e)
    {
        Logger::log("[MfrSimCommManager] Error in processMissileData: " + std::string(e.what()));
    }
}

void MfrSimCommManager::processTargetData(const char *buffer, size_t len)
{
    auto receiver = receiver_.lock();
    if (!receiver)
    {
        Logger::log("[MfrSimCommManager] Receiver not available");
        return;
    }

    try
    {
        TargetSimData data;
        std::memcpy(&data, buffer + 1, sizeof(TargetSimData));
        std::vector<char> packet(buffer, buffer + len);
        {
            std::lock_guard<std::mutex> lock(callbackMutex_);
            receiver->callBackData(packet);
        }
    }
    catch (const std::exception &e)
    {
        Logger::log("[MfrSimCommManager] Error in processTargetData: " + std::string(e.what()));
    }
}

void MfrSimCommManager::stopReceiver()
{
    isRunning_ = false;
    if (receiverThread.joinable())
    {
        receiverThread.join();
    }
    Logger::log("[MfrSimCommManager] Receiver stopped");
}