#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <iostream>
#include <byteswap.h>
#include <arpa/inet.h>
// LaunchData 구조체 정의
#pragma pack(push, 1)
struct MissileInfo
{
	uint8_t cmd;	 // 모의기 타입
	unsigned int id; // 미사일 ID
	long long x;	 // 위도
	long long y;	 // 경도
	long long z;	 // 고도
	int speed;		 // 속도
	double angle;	 // 발사각 (x,y)
	double angle2;	 // 발사각 (x,z)
	bool is_hit = false;
};

struct MissileInfoRecv
{
	int64_t LS_pos_x;
	int64_t LS_pos_y;
	int64_t LS_pos_z;
	int speed;
	double degree_xy;
	double degree_xz;

	// int64_t를 big-endian 바이트로 변환
	static void appendInt64(std::vector<uint8_t> &buffer, int64_t value)
	{
		for (int i = 0; i < 8; ++i)
		{
			buffer.push_back((value >> (56 - i * 8)) & 0xFF);
		}
	}

	// int를 big-endian 바이트로 변환
	static void appendInt(std::vector<uint8_t> &buffer, int value)
	{
		for (int i = 0; i < 4; ++i)
		{
			buffer.push_back((value >> (24 - i * 8)) & 0xFF);
		}
	}

	// double을 8바이트로 변환
	static void appendDouble(std::vector<uint8_t> &buffer, double value)
	{
		unsigned long bits;
		std::memcpy(&bits, &value, sizeof(bits));
		appendInt64(buffer, static_cast<int64_t>(bits));
	}

	// 직렬화 함수
	std::vector<uint8_t> toBytes() const
	{
		std::vector<uint8_t> buffer;
		buffer.reserve(sizeof(MissileInfo));

		appendInt64(buffer, LS_pos_x);
		appendInt64(buffer, LS_pos_y);
		appendInt64(buffer, LS_pos_z);
		appendInt(buffer, speed);
		appendDouble(buffer, degree_xy);
		appendDouble(buffer, degree_xz);

		return buffer;
	}

	// 역직렬화 함수
	static MissileInfoRecv fromBytes(const std::vector<uint8_t> &buffer)
	{
		MissileInfoRecv data;
		size_t offset = 0;

		auto readInt64 = [&](int64_t &val)
		{
			if (offset + sizeof(int64_t) <= buffer.size())
			{
				unsigned long tmp;
				std::memcpy(&tmp, buffer.data() + offset, sizeof(int64_t));
				tmp = bswap_64(tmp);
				val = static_cast<int64_t>(tmp);
				offset += sizeof(int64_t);
			}
		};

		auto readInt = [&](int &val)
		{
			if (offset + sizeof(int) <= buffer.size())
			{
				uint32_t tmp;
				std::memcpy(&tmp, buffer.data() + offset, sizeof(int));
				tmp = ntohl(tmp);
				val = static_cast<int>(tmp);
				offset += sizeof(int);
			}
		};

		auto readDouble = [&](double &val)
		{
			if (offset + sizeof(double) <= buffer.size())
			{
				unsigned long tmp;
				std::memcpy(&tmp, buffer.data() + offset, sizeof(double));
				tmp = bswap_64(tmp);
				std::memcpy(&val, &tmp, sizeof(double));
				offset += sizeof(double);
			}
		};

		readInt64(data.LS_pos_x);
		readInt64(data.LS_pos_y);
		readInt64(data.LS_pos_z);
		readInt(data.speed);
		readDouble(data.degree_xy);
		readDouble(data.degree_xz);

		return data;
	}

	// print to 10진수
	void print() const
	{
		std::cout << std::dec // 10진수 표시 모드로 설정
				  << "LS_pos_x: " << static_cast<int64_t>(LS_pos_x) << ", "
				  << "LS_pos_y: " << static_cast<int64_t>(LS_pos_y) << ", "
				  << "LS_pos_z: " << static_cast<int64_t>(LS_pos_z) << ", "
				  << "speed: " << static_cast<int>(speed) << ", "
				  << "degree_xy: " << degree_xy << ", "
				  << "degree_xz: " << degree_xz << std::endl;
	}
};
#pragma pack(pop)