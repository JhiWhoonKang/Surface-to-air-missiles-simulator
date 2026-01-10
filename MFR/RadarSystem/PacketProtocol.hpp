#pragma once
#include <cstdint>
#pragma pack(push, 1)

struct PacketHeader
{
    unsigned char cmdType;
};

struct EncodedPos2D
{
    long long latitude;
    long long longitude;
};

struct EncodedPos3D
{
    long long latitude;
    long long longitude;
    long long altitude;
};

struct Pos3D
{
    double latitude;
    double longitude;
    double altitude;
};

struct localTargetSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    Pos3D mockCoords;
    double angle;
    int speed;
    bool isHit;
};

struct localMissileSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    Pos3D mockCoords;
    double angle;
    double angle2;
    int speed;
    bool isHit;
};

struct localMockSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    Pos3D mockCoords;
    double angle;
    double angle2;
    int speed;
    bool isHit;
};

struct MockSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    EncodedPos3D mockCoords;
    int speed;
    double angle;
    double angle2;
    bool isHit;
};

struct TargetSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    EncodedPos3D mockCoords;
    int speed;
    double angle;
    double angle2;
    bool isHit;
};

struct MissileSimData // 4 + (8*3) + 8 + 4 = 40 bytes
{
    unsigned int mockId;
    EncodedPos3D mockCoords;
    int speed;
    double angle;
    double angle2;
    bool isHit;
};

struct MfrToLcMissileInfo // 4 + (8*3) + 4 + 8 + 8 + 8 + 1 = 57 bytes
{
    unsigned int id;
    EncodedPos3D missileCoords;
    int missileSpeed;
    double missileAngle;
    unsigned long long firstDetectionTime;
    unsigned long long timeToIntercept;
    bool isHit;
};

struct MfrToLcTargetInfo // 4 + (8*3) + 4 + 8 + 8 + 1 + 1 = 50 bytes
{
    unsigned int id;
    EncodedPos3D targetCoords;
    int targetSpeed;
    double targetAngle;
    double targetAngle2; // 고도각
    unsigned long long firstDetectionTime;
    unsigned char prioirty;
    bool isHit;
};

enum recvPacketType : uint8_t
{
    SIM_MOCK_DATA = 0x01,
    STATUS_REQ = 0x11,
    MODE_CHANGE = 0x12,
    LC_INIT_RES = 0x13
};

enum sendPacketType : uint8_t
{
    STATUS_RES = 0x21,
    DETECTED_INFO = 0x22,
    LC_INIT_REQ = 0x23
};

enum RadarMode : uint8_t
{
    ANGLE_MODE = 0x01,
    ROTATION_MODE = 0x02
};

enum isPriorityMode : uint8_t
{
    PRIORITY_TARGET = 0x01,
    CUSTOMIZE_TARGET = 0x02
};

struct MfrStatus
{
    unsigned int radarId;
    EncodedPos3D radarPos;
    RadarMode radarMode;
    double radarAngle;
};

struct LcInitData
{
    unsigned int radarId;
    Pos3D lcCoord;
};

struct ReqLcInitData
{
    unsigned int radarId;
};

struct ModeCahngeData // 4 + 1 + 1 + 4  = 10 bytes
{
    unsigned int radarId;
    unsigned char modeData;
    unsigned char isPriorityMode;
    unsigned int targetId;
};