//#pragma once
//#include "CommonTypes.h"
//
//namespace LC {
//    enum class CommandType : uint8_t {
//        INFO = 0,                       // 정보 요청
//    };
//
//    struct InfoPacket {
//        unsigned int id;                // 발사통제기 식별자
//        Pos2D position;                 // 위치
//    };
//
//    struct Message {
//        CommandType commandType;        // 명령 종류
//        InfoPacket data;                // 명령 또는 응답에 사용될 정보
//
//    };
//}