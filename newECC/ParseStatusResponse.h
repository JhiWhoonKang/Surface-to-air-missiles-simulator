#pragma once
#include "ReceiveMessageTypes.h"

// STATUS_REQUEST 응답 패킷 파서
ParsedStatusResponse ParseStatusResponse(const char* buffer, size_t len);
