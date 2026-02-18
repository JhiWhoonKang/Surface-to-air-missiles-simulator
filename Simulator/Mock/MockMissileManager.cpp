#include <iostream>
#include "MockMissileManager.h"
#include "CommonPacket.h"
#include "MissileInfo.h"
#include "MockMissile.h"

// 생성자 정의
MockMissileManager::MockMissileManager(std::shared_ptr<MockTargetManager> target_manager,
									   std::shared_ptr<MFRSendUDPManager> mfr_send_manager)
	: mock_target_manager_(target_manager), mfr_send_manager_(mfr_send_manager)
{
}

// 미사일 ID 업데이트
void MockMissileManager::updateMissileID()
{
	// 미사일 ID를 업데이트하는 로직을 구현합니다.
	// 예를 들어, 마지막 미사일 ID를 증가시킬 수 있습니다.
	if (last_missile_id_ >= 999)
	{
		std::cerr << "Error: Maximum missile ID reached." << std::endl;
		return;
	}
	last_missile_id_++;

	// 식별자 100을 앞에 추가하여 ID 생성
	last_missile_info_.id = 105 * 1000 + last_missile_id_;
}

void MockMissileManager::flightMissile(const MissileInfo &MissileInfo)
{
	if (is_flight_)
	{
		std::cout << "Missile is already in flight." << std::endl;
		return;
	}
	else
	{
		std::cout << "Missile flight success." << std::endl;
	}

	// flight_thread동작중인지 체크
	last_missile_info_ = MissileInfo;
	last_missile_info_.cmd = recvPacketType::SIM_MOCK_DATA;

	updateMissileID();

	std::cout << last_missile_info_.id << std::endl;

	is_flight_ = true;

	// Start the flight thread
	flight_thread_ = std::thread([this, MissileInfo]()
								 {
        MockMissile missile(last_missile_info_, mfr_send_manager_, mock_target_manager_, this->shared_from_this());
        missile.updatePosMissile(); });
	flight_thread_.detach();
}