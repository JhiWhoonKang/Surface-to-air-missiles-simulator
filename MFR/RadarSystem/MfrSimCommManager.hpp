#pragma once
#include "IReceiver.hpp"

#include "MfrConfig.h"

#include <vector>

class MfrSimCommManager {
private:
    int sockfd;
    IReceiver* receiver;
    
    MfrConfig mfrConfig;
    int simPort;

public:
    MfrSimCommManager(IReceiver* receiver);

private:
    void initMfrSimCommManager();
    bool connectToSim();
    void startUdpReceiver();
};