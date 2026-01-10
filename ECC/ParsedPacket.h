#pragma once
#include <variant>
#include "ReceiveMessageTypes.h"
#include "SendMessageTypes.h"

using ParsedPacket = std::variant<
    ParsedStatusResponse,
    RadarModeChange,
    LSModeChange,
    MissileLaunch,
	LSMove
>;
