#pragma once
#include <string>

class StepMotorController
{
private:
	int uart_fd = -1;

	const std::string device = "/dev/ttyPS1"; // UART 디바이스 파일
	unsigned int uartBaudRate = 9600;

public:
	StepMotorController();
	~StepMotorController();
	void sendCommand(const std::string &cmd);
	bool initUart();
};
