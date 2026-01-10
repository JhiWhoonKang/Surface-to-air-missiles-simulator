#include "Server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include "uart.h"

Server::Server(const std::string &address, int port)
	: serverAddress(address), port(port), serverSocket(-1), running(false)
{
}

Server::~Server()
{
	stop();
}

void Server::start()
{
	if (running)
	{
		std::cout << "Server is already running." << std::endl;
		return;
	}

	// 소켓 생성
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		std::cerr << "Failed to create socket" << std::endl;
		return;
	}

	// 소켓 옵션 설정 (재사용 가능)
	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 서버 주소 설정
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	if (serverAddress == "0.0.0.0")
	{
		serverAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);
	}

	// 소켓 바인딩
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Failed to bind socket to " << serverAddress << ":" << port << std::endl;
		close(serverSocket);
		return;
	}

	// 리스닝 시작
	if (listen(serverSocket, 5) < 0)
	{
		std::cerr << "Failed to listen on socket" << std::endl;
		close(serverSocket);
		return;
	}

	running = true;
	std::cout << "Server started on " << serverAddress << ":" << port << std::endl;

	// 서버 스레드 시작
	serverThread = std::thread(&Server::serverLoop, this);
}

void Server::stop()
{
	if (!running)
	{
		return;
	}

	running = false;
	std::cout << "Server stopping..." << std::endl;

	// 서버 소켓 닫기
	if (serverSocket != -1)
	{
		close(serverSocket);
		serverSocket = -1;
	}

	// 서버 스레드 종료 대기
	if (serverThread.joinable())
	{
		serverThread.join();
	}

	// 모든 클라이언트 스레드 종료 대기
	for (auto &thread : clientThreads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	clientThreads.clear();

	std::cout << "Server stopped." << std::endl;
}

void Server::restart()
{
	std::cout << "Server restarting..." << std::endl;
	stop();
	start();
}

void Server::serverLoop()
{
	while (running)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		// 클라이언트 연결 수락
		int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

		if (clientSocket < 0)
		{
			if (running)
			{
				std::cerr << "Failed to accept client connection" << std::endl;
			}
			continue;
		}

		// 클라이언트 정보 출력
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
		std::cout << "Client connected from " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

		// 클라이언트 처리를 위한 새 스레드 생성
		clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
	}
}

void Server::handleClient(int clientSocket)
{
	StepMotorController motor;
	motor.initUart();
	char buffer[1024];

	while (running)
	{
		memset(buffer, 0, sizeof(buffer));

		// 클라이언트로부터 데이터 수신
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

		if (bytesReceived <= 0)
		{
			// 연결 종료 또는 오류
			break;
		}

		std::string message(buffer, bytesReceived);
		std::cout << "Received: " << message << std::endl;

		// 응답 전송
		send(clientSocket, message.c_str(), message.length(), 0);
		motor.sendCommand(message.c_str());
	}

	std::cout << "Client disconnected" << std::endl;
	close(clientSocket);
}