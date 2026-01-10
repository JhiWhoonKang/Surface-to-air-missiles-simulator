#include <iostream>
#include <csignal>
#include <string>
#include <thread>
#include <chrono>
#include "Server.h"
#include "uart.h"

// 전역 서버 객체 (시그널 핸들러에서 사용)
Server *globalServer = nullptr;

// 시그널 핸들러 (Ctrl+C로 서버 종료)
void signalHandler(int signal)
{
	if (globalServer)
	{
		std::cout << "\nReceived signal " << signal << ". Shutting down server..." << std::endl;
		globalServer->stop();
	}
	exit(0);
}

int main()
{
	StepMotorController motorController;
	if (!motorController.initUart())
	{
		std::cerr << "Failed to initialize UART." << std::endl;
		return 1;
	}

	std::cout << "ZynqMotor Server Starting..." << std::endl;

	// 서버 생성 (IP: 0.0.0.0, Port: 9000)
	Server server("0.0.0.0", 9000);
	globalServer = &server;

	// 시그널 핸들러 등록
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	// 서버 시작
	server.start();

	if (server.isRunning())
	{
		std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;

		// 서버가 실행 중인 동안 대기
		while (server.isRunning())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	else
	{
		std::cerr << "Failed to start server." << std::endl;
		return 1;
	}

	return 0;
}