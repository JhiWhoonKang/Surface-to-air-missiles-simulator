#pragma once
#include "IReceiver.h"
#include <winsock2.h>
#include <process.h> 
#include "MAP_TCP.h"
#include <memory>

class ECC_TCP {
public:
    ECC_TCP();
    ~ECC_TCP();

    bool connect(const char* ip, int port);
    void send(const char* data, int len);
    void startReceiving();
    void stop();
    void registerReceiver(IReceiver* receiver);

private:
    SOCKET m_sock = INVALID_SOCKET;
    IReceiver* m_receiver = nullptr;
    bool m_bRunning = false;
    HANDLE m_hThread = nullptr;
    std::unique_ptr<MAP_TCP> map_tcp;

    static unsigned __stdcall recvThread(void* arg);
};
