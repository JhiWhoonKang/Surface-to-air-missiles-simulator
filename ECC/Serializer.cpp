// Serializer.cpp
#include "pch.h"
#include "Serializer.h"
#include <cstring>  // for std::memcpy

std::vector<uint8_t> SerializeRadarModeChange(const RadarModeChange& msg)
{
    std::vector<uint8_t> buffer(sizeof(RadarModeChange));
    std::memcpy(buffer.data(), &msg, sizeof(RadarModeChange));
    return buffer;
}

std::vector<uint8_t> SerializeLSModeChange(const LSModeChange& msg)
{
    std::vector<uint8_t> buffer(sizeof(LSModeChange));
    std::memcpy(buffer.data(), &msg, sizeof(LSModeChange));
    return buffer;
}

std::vector<uint8_t> SerializeMissileLaunch(const MissileLaunch& msg)
{
    std::vector<uint8_t> buffer(sizeof(MissileLaunch));
    std::memcpy(buffer.data(), &msg, sizeof(MissileLaunch));
    return buffer;
}
std::vector<uint8_t> SerializeLSMove(const LSMove& msg)
{
	std::vector<uint8_t> buffer(sizeof(LSMove));
	std::memcpy(buffer.data(), &msg, sizeof(LSMove));
	return buffer;
}

std::vector<uint8_t> SerializeStatusRequest(const StatusRequest& msg)
{
    std::vector<uint8_t> buffer(sizeof(StatusRequest));
    std::memcpy(buffer.data(), &msg, sizeof(StatusRequest));
    return buffer;
}
