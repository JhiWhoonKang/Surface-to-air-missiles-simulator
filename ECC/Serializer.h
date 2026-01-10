// Serializer.h
#pragma once
#include <vector>
#include <cstdint>
#include "SendMessageTypes.h"  // 송신 메시지 구조체 정의

// 각 메시지별 직렬화 함수
std::vector<uint8_t> SerializeRadarModeChange(const RadarModeChange& msg);
std::vector<uint8_t> SerializeLSModeChange(const LSModeChange& msg);
std::vector<uint8_t> SerializeMissileLaunch(const MissileLaunch& msg);
std::vector<uint8_t> SerializeStatusRequest(const StatusRequest& msg);
std::vector<uint8_t> SerializeLSMove(const LSMove& msg);