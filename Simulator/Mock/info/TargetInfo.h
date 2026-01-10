#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct TargetInfo
{
	uint8_t cmd;	 // 모의기 타입
	unsigned int id; // 미사일 ID
	long long x;	 // 위도
	long long y;	 // 경도
	long long z;	 // 고도
	int speed;		 // 속도
	double angle;	 // 발사각
	double angle2;	 // 고도각
	bool is_hit = false;
};
#pragma pack(pop)