#ifndef MFR_SEND_UDP_MANAGER_H
#define MFR_SEND_UDP_MANAGER_H

#include <string>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class MFRSendUDPManager
{
private:
	int mfr_socket_;				 // UDP 소켓 파일 디스크립터
	struct sockaddr_in client_addr_; // 클라이언트 주소 구조체

public:
	MFRSendUDPManager(/* args */);
	~MFRSendUDPManager();

	bool MFRSocketOpen(const std::string &ip, int port);
	bool sendData(const char *data, int dataSize);
};

#endif // MFR_SEND_UDP_MANAGER_H