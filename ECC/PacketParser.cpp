#pragma once
#include "pch.h"
#include "PacketParser.h"
#include "Deserializer.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept>

static void DebugPrintHex(const char* buffer, size_t length)
{
#ifdef _DEBUG // 릴리즈 모드에서는 출력 안 되게 막기
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer);

    std::cout << "[PacketParser::Parse] Received " << length << " bytes:" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    for (size_t i = 0; i < length; ++i)
    {
        // 1바이트씩 16진수로 출력 (02X 포맷)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]) << " ";

        // 16바이트마다 줄바꿈
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl; // 다시 10진수 모드로 복귀
    std::cout << "-----------------------------------------------------" << std::endl;
#endif
}

ParsedPacket PacketParser::Parse(const char* buffer, size_t length)
{
    DebugPrintHex(buffer, length);

    if (length == 0) {
        throw std::runtime_error("Empty packet buffer");
    }

    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer);
    CommandType type = static_cast<CommandType>(data[0]);

    switch (type)
    {
    case CommandType::STATUS_RESPONSE: {
        std::vector<RadarStatus> radars;
        std::vector<LCStatus> lcs;
        std::vector<LSStatus> lss;
        std::vector<TargetStatus> targets;
        std::vector<MissileStatus> missiles;

        bool ok = DeserializeStatusResponse(
            data, length, radars, lcs, lss, targets, missiles
        );

        if (!ok) {
            throw std::runtime_error("Failed to deserialize StatusResponse");
        }

        ParsedStatusResponse result;
        result.radarList = std::move(radars);
        result.lcList = std::move(lcs);
		result.lsList = std::move(lss);
        result.targetList = std::move(targets);
        result.missileList = std::move(missiles);

        return result;
    }

    case CommandType::RADAR_MODE_CHANGE_ACK: {
        RadarModeChange cmd{};
        if (!DeserializeRadarModeAck(data, length, reinterpret_cast<RadarModeChangeAck&>(cmd))) {
            throw std::runtime_error("Failed to deserialize RadarModeChange");
        }
        return cmd;
    }

    case CommandType::LS_MODE_CHANGE_ACK: {
        LSModeChange cmd{};
        if (!DeserializeLSModeAck(data, length, reinterpret_cast<LSModeChangeAck&>(cmd))) {
            throw std::runtime_error("Failed to deserialize LSModeChange");
        }
        return cmd;
    }

    case CommandType::MISSILE_LAUNCH_ACK: {
        MissileLaunch cmd{};
        if (!DeserializeMissileAck(data, length, reinterpret_cast<MissileLaunchAck&>(cmd))) {
            throw std::runtime_error("Failed to deserialize MissileLaunch");
        }
        return cmd;
    }
	case CommandType::LS_MOVE_ACK: {
		LSMove cmd{};
		if (!DeserializeLSMoveAck(data, length, reinterpret_cast<LSMoveAck&>(cmd))) {
			throw std::runtime_error("Failed to deserialize LSMove");
		}
		return cmd;
	}
    default:
        throw std::runtime_error("Unknown CommandType in packet");
    }
}
