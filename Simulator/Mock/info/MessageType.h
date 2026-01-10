#pragma once
#include <cstdint>
#pragma pack(push, 1)
enum recvPacketType : uint8_t
{
	SIM_DATA = 0x01,
	STATUS_REQ = 0x11,
	MODE_CHANGE = 0x12,
	LC_INIT_RES = 0x13
};
#pragma pack(pop)