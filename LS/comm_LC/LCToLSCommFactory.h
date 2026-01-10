#pragma once

#include <memory>
#include <string>
#include "LCToLSCommInterface.h"
#include "SerialReceiverInterface.h"

std::unique_ptr<LCToLSCommInterface> createLCToLSComm(
    SerialReceiverInterface& receiver, 
    const std::string& configPath);
