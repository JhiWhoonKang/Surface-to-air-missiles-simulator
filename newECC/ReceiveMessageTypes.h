#pragma once

#include "MessageCommon.h"
#include "RadarStatus.h"
#include "LSStatus.h"
#include "LCStatus.h"
#include "MissileStatus.h"
#include "TargetStatus.h"
#include <vector>
#pragma pack(push, 1)

// 공통 응답 헤더 (상태전송 메시지용)
struct StatusHeader {
    CommandType commandType = CommandType::STATUS_RESPONSE;
    uint8_t num_radar;
    uint8_t num_lc;
    uint8_t num_ls;
    uint8_t num_target;
    uint8_t num_missile;
};

#pragma pack(pop)

// 상태 수신 후 구조체로 해석한 결과
struct ParsedStatusResponse {
    std::vector<RadarStatus> radarList;
    std::vector<LCStatus> lcList;
    std::vector<LSStatus> lsList;
    std::vector<TargetStatus> targetList;
    std::vector<MissileStatus> missileList;
};
