#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
#include "../MFR/info/PacketProtocol.h"

#pragma pack(push, 1) // 1바이트 정렬 (필수: 패딩 문제 방지)

// 1. 패킷의 머리말 (검증용 정보)
struct PacketHeader
{
    uint32_t magic;       // 0xA1B2C3D4 (프로토콜 식별)
    uint32_t seqID;       // 패킷 순서 (Loss 확인)
    uint32_t payloadCRC;  // 데이터 무결성 (깨짐 확인)
    uint32_t count;       // 이 패킷에 들어있는 Target 개수
};

enum recvPacketType : uint8_t
{
    SIM_MOCK_DATA = 0x01,
    STATUS_REQ = 0x11,
    MODE_CHANGE = 0x12,
    LC_INIT_RES = 0x13
};
#pragma pack(pop)

// 3. CRC-32 계산 함수 (테이블 생성 방식 - 빠르고 가벼움)
inline uint32_t calculateCRC32(const void* data, size_t length) {
    static std::array<uint32_t, 256> table;
    static bool initialized = false;
    if (!initialized) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int j = 0; j < 8; j++) c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        initialized = true;
    }
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < length; i++) {
        crc = table[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}