#include "pch.h"
#include "PacketParser.h"
#include "Deserializer.h"
#include <cstring>
#include <stdexcept>

ParsedPacket PacketParser::Parse(const char* buffer, size_t length)
{

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
