#include "MFRSendUDPManager.h"

MFRSendUDPManager::MFRSendUDPManager(/* args */)
{
	// Constructor implementation
}

MFRSendUDPManager::~MFRSendUDPManager()
{
	// Destructor implementation
}

bool MFRSendUDPManager::MFRSocketOpen(const std::string &ip, int port)
{
	// UDP 소켓 생성
	mfr_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (mfr_socket_ < 0)
	{
		perror("socket");
		return false;
	}

	// 서버 주소 설정
	memset(&client_addr_, 0, sizeof(client_addr_));
	client_addr_.sin_family = AF_INET;
	client_addr_.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &client_addr_.sin_addr) <= 0)
	{
		perror("inet_pton");
		close(mfr_socket_);
		mfr_socket_ = -1;
		return false;
	}

	std::cout << "MFRSocketOpen: Socket opened for IP " << ip << " on port " << port << std::endl;
	return true;
}

bool MFRSendUDPManager::sendData(const char *data, int dataSize)
{
	if (mfr_socket_ < 0)
	{
		std::cerr << "Socket is not open. Call MFRSocketOpen first." << std::endl;
		return false;
	}

	// 데이터 전송
	ssize_t sent_bytes = sendto(mfr_socket_, data, dataSize, 0,
								(struct sockaddr *)&client_addr_, sizeof(client_addr_));
	if (sent_bytes < 0)
	{
		perror("sendto");
		return false;
	}

	// std::cout << "missile sendData: Sent " << sent_bytes << " bytes to "
	// 		  << inet_ntoa(client_addr_.sin_addr) << ":" << ntohs(client_addr_.sin_port) << std::endl;
	return true;
}

void MFRSendUDPManager::sendTargetBatch(const std::vector<TargetSimData>& allTargets)
{
    static uint32_t globalSeqID = 0; // 패킷 순서 번호 (계속 증가)
    
    // UDP 패킷 하나당 보낼 표적 개수 (MTU 1500 byte 고려)
    // TargetSimData가 약 40~50바이트라면, 30개 정도가 적당 (30 * 50 = 1500 이하)
    const size_t TARGETS_PER_PACKET = 30;
    size_t total = allTargets.size();

    for (size_t i = 0; i < total; i += TARGETS_PER_PACKET)
    {
        // 1. 이번 패킷에 담을 개수 계산
        size_t count = std::min(TARGETS_PER_PACKET, total - i);
        size_t payloadSize = count * sizeof(TargetSimData);
        size_t totalPacketSize = sizeof(PacketHeader) + payloadSize;

        // 2. 버퍼 생성 (Header + Payload)
        std::vector<char> buffer(totalPacketSize);
        
        // 포인터 설정
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer.data());
        TargetSimData* payload = reinterpret_cast<TargetSimData*>(buffer.data() + sizeof(PacketHeader));

        // 3. 데이터 복사 (Payload 채우기)
        // allTargets 벡터의 i번째부터 count개만큼 복사
        std::memcpy(payload, &allTargets[i], payloadSize);

        // 4. 헤더 작성 (검증 정보 기입)
        header->magic = 0xA1B2C3D4;        // 매직 넘버
        header->seqID = globalSeqID++;     // 순서 번호 증가
        header->count = (uint32_t)count;   // 개수
        
        // [핵심] CRC 계산: Payload 데이터에 대해서만 계산하여 헤더에 기록
        header->payloadCRC = calculateCRC32(payload, payloadSize);

        // 5. 기존 sendData 함수 호출 (char* 로 변환하여 전송)
        sendData(buffer.data(), (int)buffer.size());
        
        // (옵션) CPU 과부하 방지를 위한 미세 딜레이
        // std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}