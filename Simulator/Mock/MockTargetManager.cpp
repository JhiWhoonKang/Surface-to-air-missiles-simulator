#include "MockTargetManager.h"
#include "MessageType.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// 생성자 수정: MFRSendUDPManager 포인터를 받도록 변경
MockTargetManager::MockTargetManager(std::shared_ptr<MFRSendUDPManager> mfr_send_manager)
	: mfr_send_manager_(mfr_send_manager)
{
	// Constructor implementation
}

MockTargetManager::~MockTargetManager()
{
	// Destructor implementation
}

void MockTargetManager::RaedTargetIni()
{
	std::ifstream file("../Config/target_list.ini");
	if (!file.is_open())
	{
		std::cerr << "Failed to open target_list.ini" << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;
		std::vector<std::string> tokens;

		while (std::getline(ss, token, ','))
		{
			tokens.push_back(token);
		}

		if (tokens.size() == 7)
		{
			try
			{
				unsigned int id = std::stoul(tokens[0]);
				long long x = std::stoul(tokens[1]);
				// std::cout << "x : " << x << std::endl;
				long long y = std::stoul(tokens[2]);
				// std::cout << "y : " << y << std::endl;
				long long z = std::stoul(tokens[3]);
				// std::cout << "z : " << z << std::endl;
				double angle = std::stod(tokens[4]);
				double angle2 = std::stod(tokens[5]);
				int speed = std::stoi(tokens[6]);

				// TargetInfo 객체 생성 및 데이터 설정
				TargetInfo targetInfo;
				targetInfo.cmd = recvPacketType::SIM_DATA;
				targetInfo.id = id;
				targetInfo.x = x;
				targetInfo.y = y;
				targetInfo.z = z;
				targetInfo.angle = angle;
				targetInfo.angle2 = angle2;
				targetInfo.speed = speed;

				// MockTarget 객체 생성 및 TargetInfo, MFRSendUDPManager 설정
				std::shared_ptr<MockTarget> target = std::make_shared<MockTarget>(targetInfo, mfr_send_manager_);

				// 타겟 추가
				addTarget(target);
			}
			catch (const std::invalid_argument &e)
			{
				std::cerr << "Invalid argument: " << e.what() << std::endl;
			}
			catch (const std::out_of_range &e)
			{
				std::cerr << "Out of range: " << e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << "Invalid line format: " << line << std::endl;
		}
	}

	file.close();

	// std::cout << "Target list loaded successfully." << std::endl;
}

void MockTargetManager::addTarget(std::shared_ptr<MockTarget> &target)
{
	targets.push_back(target);
}

void MockTargetManager::removeTarget(const std::vector<TargetInfo> &target_list)
{
	// 무효한 타겟 제거
	targets.erase(
		std::remove_if(targets.begin(), targets.end(),
					   [](const std::shared_ptr<MockTarget> &target)
					   {
						   return !target;
					   }),
		targets.end());

	for (const auto &target : target_list)
	{
		targets.erase(
			std::remove_if(targets.begin(), targets.end(),
						   [&target](const std::shared_ptr<MockTarget> &t)
						   {
							   return t && t->getTargetInfo().id == target.id;
						   }),
			targets.end());
	}
}

void MockTargetManager::flitghtTarget()
{
	std::vector<TargetInfo> target_info_list;
	// 유효한 타겟만 업데이트
	for (auto &target : targets)
	{
		if (target)
		{
			auto tmp = target->updatePos();
			target_info_list.push_back(tmp);
		}
	}

	for (const auto target_info : target_info_list)
	{
		char buffer[1024];
		std::memcpy(buffer, &target_info, sizeof(target_info));
		mfr_send_manager_->sendData(buffer, sizeof(target_info));
		std::cout << "test id : " << target_info.id << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int MockTargetManager::downTargetStatus(const MissileInfo &missileInfo)
{
	int down_count = 0;
	std::vector<TargetInfo> down_targets;
	for (auto &target : targets)
	{
		if (target->downTargetStatus(missileInfo))
		{
			++down_count;
			down_targets.push_back(target->getTargetInfo());
			target->updatePos();
		}
	}

	removeTarget(down_targets);

	return down_count;
}
