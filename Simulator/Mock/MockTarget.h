#ifndef MOCK_TARGET_H
#define MOCK_TARGET_H

#include <string>
#include <memory>
#include <chrono>

#include "TargetInfo.h"
#include "MissileInfo.h"
#include "MFRSendUDPManager.h"

class MockTarget
{
public:
	MockTarget(const TargetInfo &target_info, std::shared_ptr<MFRSendUDPManager> mfr_send_manager);
	~MockTarget();

	TargetInfo updatePos(); // 위치 업데이트
	bool downTargetStatus(const MissileInfo &missileInfo);
	TargetInfo getTargetInfo() const { return target_info_; }

private:
	TargetInfo target_info_;
	std::shared_ptr<MFRSendUDPManager> mfr_send_manager_ = nullptr;

	std::chrono::steady_clock::time_point last_time_;
	double total_elapsed_;
	double accumulated_distance_;

	void sendData(); // 데이터를 전송
};

#endif // MOCK_TARGET_H