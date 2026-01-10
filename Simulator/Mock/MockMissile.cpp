#include <cmath>
#include <thread>
#include <iostream>
#include <cstring>
#include <memory>
#include <chrono>

#include "MockMissile.h"

constexpr double DEGREE_TO_INT = 1e7; // 실수 → 정수 저장시 스케일
constexpr double METERS_PER_DEGREE_LAT = 111320.0;

MockMissile::MockMissile(const MissileInfo &missile_info,
						 std::shared_ptr<MFRSendUDPManager> mfr_send_manager,
						 std::shared_ptr<MockTargetManager> mock_target_manager,
						 std::shared_ptr<MockMissileManager> mock_missile_manager)
	: missile_info_(missile_info), mfr_send_manager_(mfr_send_manager), mock_target_manager_(mock_target_manager), mock_missile_manager_(mock_missile_manager) {}

void MockMissile::updatePosMissile()
{
	const auto start_time = std::chrono::steady_clock::now();

	const double start_lat = static_cast<double>(missile_info_.x) / DEGREE_TO_INT;
	const double start_lon = static_cast<double>(missile_info_.y) / DEGREE_TO_INT;
	const double start_alt = static_cast<double>(missile_info_.z); // 정수 고도(m)

	const double speed_mps = missile_info_.speed * 0.27778;
	const double launch_angle_rad = missile_info_.angle * M_PI / 180.0;
	const double launch_angle_z_rad = missile_info_.angle2 * M_PI / 180.0;

	while (true)
	{
		// 경과 시간 계산
		const auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - start_time;
		double elapsed_seconds = elapsed.count();

		// 이동 거리 계산
		double distance = speed_mps * elapsed_seconds;

		// 이동 거리 기반으로 새로운 위치 계산 (실수 단위)
		double delta_lat = std::cos(launch_angle_rad) * distance / METERS_PER_DEGREE_LAT;
		double meters_per_deg_lon = METERS_PER_DEGREE_LAT * std::cos(start_lat * M_PI / 180.0);
		double delta_lon = std::sin(launch_angle_rad) * distance / meters_per_deg_lon;
		double delta_alt = std::tan(launch_angle_z_rad) * distance;

		// 정수 환산
		missile_info_.x = static_cast<long long>((start_lat + delta_lat) * DEGREE_TO_INT);
		missile_info_.y = static_cast<long long>((start_lon + delta_lon) * DEGREE_TO_INT);
		missile_info_.z = static_cast<long long>(start_alt + delta_alt);

		// 출력 (4초마다)
		if (static_cast<int>(elapsed_seconds) % 4 == 0)
		{
			std::cout << "[4 sec update] Missile ID " << missile_info_.id
					  << " moved " << distance << " meters.\n";
			std::cout << " → Current lat: " << static_cast<double>(missile_info_.x) / DEGREE_TO_INT
					  << ", lon: " << static_cast<double>(missile_info_.y) / DEGREE_TO_INT
					  << ", alt: " << static_cast<double>(missile_info_.z) << " m\n";
		}

		// 명중 판정
		if (mock_target_manager_->downTargetStatus(missile_info_) > 0)
		{
			std::cout << "Missile hit target!" << std::endl;
			missile_info_.is_hit = true;
			sendData();
			mock_missile_manager_->setFlightStatus(false);
			return;
		}
		else
		{
			// auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			// std::cout << "[Current time, MockMissile]: " << std::ctime(&now_time);
			// std::cout << "Missile is still in flight.\n";
		}

		sendData();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void MockMissile::sendData()
{
	char buffer[1024];
	std::memcpy(buffer, &missile_info_, sizeof(missile_info_));
	mfr_send_manager_->sendData(buffer, sizeof(missile_info_));
}
