#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct MockInfo
{
	unsigned int id; // 미사일 ID
	float x;		 // 위도
	float y;		 // 경도
	float z;		 // 고도
	float angle;	 // 발사각
	int speed;		 // 속도
};

int main()
{
	const int port = 3001; // Listen할 포트 번호
	char buffer[1024];	   // 수신할 데이터를 저장할 버퍼

	// UDP 소켓 생성
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("socket");
		return -1;
	}

	// 서버 주소 설정
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 수신
	server_addr.sin_port = htons(port);

	// 소켓 바인딩
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind");
		close(sock);
		return -1;
	}

	std::cout << "Listening on port " << port << "..." << std::endl;

	// 데이터 수신
	while (true)
	{
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		ssize_t recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
									(struct sockaddr *)&client_addr, &client_addr_len);
		if (recv_len < 0)
		{
			perror("recvfrom");
			continue;
		}

		buffer[recv_len] = '\0'; // Null-terminate the received data

		// 수신된 데이터 출력
		MockInfo received_data;
		std::memcpy(&received_data, buffer, sizeof(received_data));
		std::cout << "Received LaunchData:" << std::endl;
		std::cout << "  ID: " << received_data.id << std::endl;
		std::cout << "  Latitude: " << received_data.x << std::endl;
		std::cout << "  Longitude: " << received_data.y << std::endl;
		std::cout << "  Altitude (z): " << received_data.z << std::endl;
		std::cout << "  Angle: " << received_data.angle << std::endl;
		std::cout << "  Speed: " << received_data.speed << std::endl;

		// 클라이언트 정보 출력 (선택 사항)
		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
		std::cout << "From: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
	}

	// 소켓 닫기
	close(sock);
	return 0;
}