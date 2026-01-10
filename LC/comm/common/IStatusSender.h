    //IStatusSender.h

    #include <vector>

    #pragma once

    #include "CommonMessage.h"

    class IStatusSender {
        
    public:
        virtual void sendStatus(const Common::CommonMessage& msg) = 0;
        virtual void sendRaw(const std::vector<uint8_t>& data) = 0;  // 추가!
        virtual ~IStatusSender() = default;
    };