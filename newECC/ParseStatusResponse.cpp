#include "pch.h"
#include "ParseStatusResponse.h"
#include <cstring>

ParsedStatusResponse ParseStatusResponse(const char* buffer, size_t len)
{
    ParsedStatusResponse result;
    size_t offset = 0;

    const StatusHeader* header = reinterpret_cast<const StatusHeader*>(buffer);
    offset += sizeof(StatusHeader);

    for (int i = 0; i < header->num_radar; ++i) {
        result.radarList.push_back(*reinterpret_cast<const RadarStatus*>(buffer + offset));
        offset += sizeof(RadarStatus);
    }

    for (int i = 0; i < header->num_ls; ++i) {
        result.lsList.push_back(*reinterpret_cast<const LSStatus*>(buffer + offset));
        offset += sizeof(LSStatus);
    }

    for (int i = 0; i < header->num_lc; ++i) {
        result.lcList.push_back(*reinterpret_cast<const LCStatus*>(buffer + offset));
        offset += sizeof(LCStatus);
    }

    for (int i = 0; i < header->num_target; ++i) {
        result.targetList.push_back(*reinterpret_cast<const TargetStatus*>(buffer + offset));
        offset += sizeof(TargetStatus);
    }

    for (int i = 0; i < header->num_missile; ++i) {
        result.missileList.push_back(*reinterpret_cast<const MissileStatus*>(buffer + offset));
        offset += sizeof(MissileStatus);
    }

    return result;
}
