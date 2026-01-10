//#pragma once
//#include "CommonTypes.h"
//
//namespace Target {
//    enum class CommandType : uint8_t {
//        INFO = 0,                       // 정보 요청
//    };
//
//    struct InfoPacket {
//        unsigned int id;                // 미사일 식별자
//        Pos2D position;                 // 위치
//        long long z;                    // 고도
//        long long speed;                // 속도
//        double angle;                   // 각도
//        int shoot_down_time;            // 위협 우선 순위
//    };
//
//    struct Message {
//        CommandType commandType;        // 명령 종류
//        std::vector<InfoPacket> data;   // 명령 또는 응답에 사용될 정보
//
//    };
//}