#include "Simulator.h"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <stdexcept>
#include <thread>

#include "Config.h"
#include "MissileInfo.h"
#include "LSRecvUDPManager.h"
#include "MFRSendUDPManager.h"
#include "CommonPacket.h"

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
	// Destructor implementation
}

bool Simulator::init()
{
	ConfigCommon config;
	if (loadConfig("../Config/Simulator.ini", config) == false)
	{
		std::cerr << "Failed to load configuration from Simulator.ini." << std::endl;
		return false;
	}

	// Constructor implementation
	ls_recv_manager_ = std::make_unique<LSRecvUDPManager>();
	mfr_send_manager_ = std::make_shared<MFRSendUDPManager>();
	if (!ls_recv_manager_ || !mfr_send_manager_)
	{
		std::cerr << "Failed to allocate memory for LSRecvUDPManager or MFRSendUDPManager." << std::endl;
		return false;
	}

	if (!ls_recv_manager_->LSSocketOpen(config.LSRecvPort))
	{
		std::cerr << "Failed to initialize ls_recv_manager socket." << std::endl;
		return false;
	}

	if (!mfr_send_manager_->MFRSocketOpen(config.MFRSendIP, config.MFRSendPort))
	{
		std::cerr << "Failed to initialize mfr_send_manager socket." << std::endl;
		return false;
	}

	// Initialize the mock target manager
	mock_target_manager_ = std::make_shared<MockTargetManager>(mfr_send_manager_);
	mock_target_manager_->RaedTargetIni();

	// Initialize the mock missile
	mock_missile_manager_ = std::make_shared<MockMissileManager>(mock_target_manager_, mfr_send_manager_);
	// mock_target_manager_->SetMockMissileManager(mock_missile_manager_);

	return true;
}

void Simulator::start()
{
	recv_thread_ = std::thread([this]()
							   {
		std::vector<uint8_t> buffer(sizeof(MissileInfoRecv));
        while (true) {
            if (ls_recv_manager_->receiveData(&buffer, sizeof(buffer)) > 0) {
				// 수신된 데이터를 LaunchData 구조체로 파싱
				MissileInfoRecv received_dat = MissileInfoRecv::fromBytes(buffer);
				received_dat.print();

				MissileInfo missile;
				missile.x = received_dat.LS_pos_x;
				missile.y = received_dat.LS_pos_y;
				missile.z = received_dat.LS_pos_z;
				missile.speed = received_dat.speed;
				missile.angle = received_dat.degree_xy;
				missile.angle2 = received_dat.degree_xz;
				mock_missile_manager_->flightMissile(missile);
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		} });
	recv_thread_.detach();

	flight_target_thread_ = std::thread([this]()
										{
		while (true)
		{
			mock_target_manager_->flitghtTarget();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} });
	flight_target_thread_.detach();
}