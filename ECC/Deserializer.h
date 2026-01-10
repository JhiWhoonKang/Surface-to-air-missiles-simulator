// Deserializer.h
#pragma once
#include <vector>
#include <cstdint>
#include "ReceiveMessageTypes.h"
#include "AckMessageTypes.h"

// 각 메시지별 역직렬화 함수 (성공 시 true 반환)
bool DeserializeRadarModeAck(const uint8_t* data, size_t len, RadarModeChangeAck& out);
bool DeserializeLSModeAck(const uint8_t* data, size_t len, LSModeChangeAck& out);
bool DeserializeMissileAck(const uint8_t* data, size_t len, MissileLaunchAck& out);
bool DeserializeLSMoveAck(const uint8_t* data, size_t len, LSMoveAck& out);
// 상태 전송 메시지 파싱 함수
bool DeserializeStatusResponse(
    const uint8_t* data, size_t len,
    std::vector<RadarStatus>& radars,
    std::vector<LCStatus>& lcs,
    std::vector<LSStatus>& lss,
    std::vector<TargetStatus>& targets,
    std::vector<MissileStatus>& missiles
);