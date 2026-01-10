#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <byteswap.h>
#include <arpa/inet.h>
// 패딩 제거
#pragma pack(push, 1)

enum class OperationMode : uint8_t
{
    WAR_MODE = 0,
    MOVE_MODE = 1,
    STOP_MODE = 2
};

enum class CommandType : uint8_t
{
    LAUNCH         = 0x31, // 발사명령
    MOVE           = 0x32, // 이동명령
    MODE_CHANGE    = 0x33, // 모드변경
    STATUS_REQUEST = 0x34  // 상태요청
};

struct MissileInfo
{
    int64_t LS_pos_x;
    int64_t LS_pos_y;
    int64_t LS_pos_z;
    int speed;
    double degree_xy;
    double degree_xz;

    // int64_t를 big-endian 바이트로 변환
    static void appendInt64(std::vector<uint8_t>& buffer, int64_t value)
    {
        for (int i = 0; i < 8; ++i) {
            buffer.push_back((value >> (56 - i * 8)) & 0xFF);
        }
    }

    // int를 big-endian 바이트로 변환
    static void appendInt(std::vector<uint8_t>& buffer, int value)
    {
        for (int i = 0; i < 4; ++i) {
            buffer.push_back((value >> (24 - i * 8)) & 0xFF);
        }
    }

    // double을 8바이트로 변환
    static void appendDouble(std::vector<uint8_t>& buffer, double value)
    {
        unsigned long bits;
        std::memcpy(&bits, &value, sizeof(bits));
        appendInt64(buffer, static_cast<int64_t>(bits));
    }

    // 직렬화 함수
    std::vector<uint8_t> toBytes() const 
    {
        std::vector<uint8_t> buffer;
        buffer.reserve(sizeof(MissileInfo));

        appendInt64(buffer, LS_pos_x);
        appendInt64(buffer, LS_pos_y);
        appendInt64(buffer, LS_pos_z);
        appendInt(buffer, speed);
        appendDouble(buffer, degree_xy);
        appendDouble(buffer, degree_xz);

        return buffer;
    }

// 역직렬화 함수
static MissileInfo fromBytes(const std::vector<uint8_t> &buffer)
{
    MissileInfo data;
    size_t offset = 0;

    auto readInt64 = [&](int64_t &val) {
        if (offset + sizeof(int64_t) <= buffer.size()) {
            unsigned long tmp;
            std::memcpy(&tmp, buffer.data() + offset, sizeof(int64_t));
            tmp = bswap_64(tmp);
            val = static_cast<int64_t>(tmp);
            offset += sizeof(int64_t);
        }
    };

    auto readInt = [&](int &val) {
        if (offset + sizeof(int) <= buffer.size()) {
            uint32_t tmp;
            std::memcpy(&tmp, buffer.data() + offset, sizeof(int));
            tmp = ntohl(tmp);
            val = static_cast<int>(tmp);
            offset += sizeof(int);
        }
    };

    auto readDouble = [&](double &val) {
        if (offset + sizeof(double) <= buffer.size()) {
            unsigned long tmp;
            std::memcpy(&tmp, buffer.data() + offset, sizeof(double));
            tmp = bswap_64(tmp);
            std::memcpy(&val, &tmp, sizeof(double));
            offset += sizeof(double);
        }
    };

    readInt64(data.LS_pos_x);
    readInt64(data.LS_pos_y);
    readInt64(data.LS_pos_z);
    readInt(data.speed);
    readDouble(data.degree_xy);
    readDouble(data.degree_xz);

    return data;
}
    // print
    void print() const
    {
        std::cout << "LS_pos_x: " << LS_pos_x << ", "
                  << "LS_pos_y: " << LS_pos_y << ", "
                  << "LS_pos_z: " << LS_pos_z << ", "
                  << "speed: " << speed << ", "
                  << "degree_xy: " << degree_xy << ", "
                  << "degree_xz: " << degree_xz << std::endl;
    }
};

struct LaunchCommand
{
    double launch_angle_xy;
    double launch_angle_xz;
    long long start_x;
    long long start_y;
    long long start_z;
};

struct MoveCommand
{
    int64_t new_x;
    int64_t new_y;
};

struct ModeChangeCommand
{
    OperationMode new_mode;
};

struct LauncherMessage
{
    CommandType type;
    uint32_t launcher_id;
    union {
        LaunchCommand launch;
        MoveCommand move;
        ModeChangeCommand mode_change;
    };
};

// 정렬 복원
#pragma pack(pop)