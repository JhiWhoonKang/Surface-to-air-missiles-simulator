//#pragma once
//#include "CommonTypes.h"
//
//namespace LS {
//    enum class CommandType : uint8_t {
//        INFO = 0,                       // 정보 요청
//        MODE_CHANGE = 1,                // 모드 변경 요청
//        MISSILE_LAUNCH = 2,               // 미사일 발사 요청
//        MOVE_POS = 3,                   // 위치 변경 요청
//    };
//
//    enum class Mode : uint8_t {
//        STOP_MODE = 0,
//        MOVE_MODE = 1,
//        WAR_MODE = 2
//    };
//
//    struct InfoPacket {
//        unsigned int id;                // 발사대 식별자
//        Pos2D position;                 // 위치
//        double angle;                   // 발사 각
//        Mode mode;                      // 현재 모드
//        int speed;			// 미사일 발사 속도
//
//    };
//
//    struct Message {
//        CommandType commandType;        // 명령 종류
//        InfoPacket data;                // 명령 또는 응답에 사용될 정보
//    };
//}