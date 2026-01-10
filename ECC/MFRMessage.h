//#pragma once
//#include "CommonTypes.h"
//
//namespace MFR
//{
//    enum class CommandType : uint8_t {
//        INFO = 0,                       // 정보 요청
//        MODE_CHANGE = 1,                // 모드 변경 요청
//        ANGLE_CHANGE = 2,               // 각도 변경 요청
//    };
//
//    enum class Mode : uint8_t {
//        STOP_MODE = 0,
//        ROTATE_MODE = 1
//    };
//
//    struct InfoPacket {
//        unsigned int id;                // 발사대 식별자
//        Pos2D position;                 // 위치
//        double angle;                   // 탐지 각
//        Mode mode;                      // 현재 모드
//    };
//
//    struct Message {
//        CommandType commandType;        // 명령 종류
//        InfoPacket data;                // 명령 또는 응답에 사용될 정보
//
//    };
//
//}