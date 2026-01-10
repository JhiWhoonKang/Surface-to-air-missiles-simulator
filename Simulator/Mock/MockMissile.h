#pragma once

#include "MockTargetManager.h"
#include "MissileInfo.h"
#include "MFRSendUDPManager.h"
#include "MockMissileManager.h"

class MockMissile
{
public:
	MockMissile(const MissileInfo &missile_info, std::shared_ptr<MFRSendUDPManager> mfr_send_manager, std::shared_ptr<MockTargetManager> mock_target_manager, std::shared_ptr<MockMissileManager> mock_missile_manager);
	~MockMissile() {}

	void updatePosMissile(); // 미사일 목표 설정

private:
	MissileInfo missile_info_;
	std::shared_ptr<MFRSendUDPManager> mfr_send_manager_;
	std::shared_ptr<MockTargetManager> mock_target_manager_;
	std::shared_ptr<MockMissileManager> mock_missile_manager_;

	void sendData(); // 데이터를 전송
};