// Deserializer.cpp
#include "pch.h"
#include "Deserializer.h"
#include <cstring>
#include <iostream>
bool DeserializeRadarModeAck(const uint8_t* data, size_t len, RadarModeChangeAck& out)
{
    if (len < sizeof(RadarModeChangeAck)) return false;
    std::memcpy(&out, data, sizeof(RadarModeChangeAck));
    return true;
}

bool DeserializeLSModeAck(const uint8_t* data, size_t len, LSModeChangeAck& out)
{
    if (len < sizeof(LSModeChangeAck)) return false;
    std::memcpy(&out, data, sizeof(LSModeChangeAck));
    return true;
}

bool DeserializeMissileAck(const uint8_t* data, size_t len, MissileLaunchAck& out)
{
    if (len < sizeof(MissileLaunchAck)) return false;
    std::memcpy(&out, data, sizeof(MissileLaunchAck));
    return true;
}
bool DeserializeLSMoveAck(const uint8_t* data, size_t len, LSMoveAck& out)
{
	if (len < sizeof(LSMoveAck)) return false;
	std::memcpy(&out, data, sizeof(LSMoveAck));
	return true;
}
bool DeserializeStatusResponse(
    const uint8_t* data, size_t len,
    std::vector<RadarStatus>& radars,
    std::vector<LCStatus>& lcs,
    std::vector<LSStatus>& lss,
    std::vector<TargetStatus>& targets,
    std::vector<MissileStatus>& missiles
)
{

    size_t offset = 0;

    // CommandType (1바이트) skip
    if (len < offset + 1) return false;
    offset += 1;

    // radar, lc, ls count (각 1바이트, 사용하지 않으면 skip)
    if (len < offset + 3) return false;
    uint8_t radarCount = data[offset++];
    uint8_t lcCount = data[offset++];
    uint8_t lsCount = data[offset++];

    // target/missile count
    if (len < offset + 2) return false;
    uint8_t targetCount = data[offset++];
    uint8_t missileCount = data[offset++];

    // RadarStatus
    radars.clear();
    radars.reserve(radarCount);
    for (uint8_t i = 0; i < radarCount; ++i) {
        if (len < offset + sizeof(RadarStatus)) return false;
        RadarStatus radar;
        std::memcpy(&radar, data + offset, sizeof(RadarStatus));
        radars.push_back(radar);
        offset += sizeof(RadarStatus);
    }

    // LSStatus
    lss.clear();
    lss.reserve(lsCount);
    for (uint8_t i = 0; i < lsCount; ++i) {
        if (len < offset + sizeof(LSStatus)) return false;
        LSStatus ls;
        std::memcpy(&ls, data + offset, sizeof(LSStatus));
        lss.push_back(ls);
        offset += sizeof(LSStatus);
    }

    // LCStatus
    lcs.clear();
    lcs.reserve(lcCount);
    for (uint8_t i = 0; i < lcCount; ++i) {
        if (len < offset + sizeof(LCStatus)) return false;
        LCStatus lc;
        std::memcpy(&lc, data + offset, sizeof(LCStatus));
        lcs.push_back(lc);
        offset += sizeof(LCStatus);
    }

    // MissileStatus
    missiles.clear();
    missiles.reserve(missileCount);
    for (uint8_t i = 0; i < missileCount; ++i) {
        if (len < offset + sizeof(MissileStatus)) return false;
        MissileStatus missile;
        std::memcpy(&missile, data + offset, sizeof(MissileStatus));
        missiles.push_back(missile);
        offset += sizeof(MissileStatus);
    }

    // TargetStatus
    targets.clear();
    targets.reserve(targetCount);
    for (uint8_t i = 0; i < targetCount; ++i) {
        if (len < offset + sizeof(TargetStatus)) return false;
        TargetStatus target;
        std::memcpy(&target, data + offset, sizeof(TargetStatus));
        targets.push_back(target);
        offset += sizeof(TargetStatus);
    }

    return true;
 //   size_t offset = 0;

 //   if (len < sizeof(StatusHeader)) return false;

 //   const StatusHeader* header = reinterpret_cast<const StatusHeader*>(data);
 //   offset += sizeof(StatusHeader);

 //   // 총 예상 바이트 수
 //   size_t expectedSize =
 //       sizeof(StatusHeader)
 //       + header->num_radar * sizeof(RadarStatus)
 //       + header->num_lc * sizeof(LCStatus)
 //       + header->num_ls * sizeof(LSStatus)
 //       + header->num_target * sizeof(TargetStatus)
 //       + header->num_missile * sizeof(MissileStatus);
	//std::cout << "[디버그] 예상 크기: " << expectedSize << ", 실제 크기: " << len << "\n";

 //   //if (len < expectedSize) return false;

 //   // 안전한 배열 복사 람다
 //   auto read_array = [&](auto& vec, size_t count, size_t itemSize) -> bool {
 //       if (offset + count * itemSize > len) return false;

 //       for (size_t i = 0; i < count; ++i) {
 //           typename std::remove_reference_t<decltype(vec)>::value_type item{};
 //           std::memcpy(&item, data + offset, itemSize);
 //           vec.push_back(item);
 //           offset += itemSize;
 //       }
 //       return true;
 //       };

 //   if (!read_array(radars, header->num_radar, sizeof(RadarStatus))) return false;
 //   if (!read_array(lcs, header->num_lc, sizeof(LCStatus))) return false;
 //   if (!read_array(lss, header->num_ls, sizeof(LSStatus))) return false;
 //   if (!read_array(targets, header->num_target, sizeof(TargetStatus))) return false;
 //   if (!read_array(missiles, header->num_missile, sizeof(MissileStatus))) return false;

 //   return true;
}

