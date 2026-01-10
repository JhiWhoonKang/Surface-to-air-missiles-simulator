// IReceiverCallback.h
#pragma once
#include "CommonMessage.h"

class IReceiverCallback {
public:
    virtual ~IReceiverCallback() = default;
    virtual void onMessage(const Common::CommonMessage& msg) = 0;
};