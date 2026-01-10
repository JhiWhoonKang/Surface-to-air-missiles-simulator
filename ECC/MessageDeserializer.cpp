//#include "pch.h"
//#include "MessageDeserializer.h"
//
//CommonMessage DeserializeCommonMessage(const uint8_t* data, size_t length)
//{
//    CommonMessage msg;
//    size_t offset = 0;
//
//    // Header: messageType, infoType, ok, payloadCount
//    msg.messageType = static_cast<MessageType>(data[offset++]);
//    msg.infoType = static_cast<InfoType>(data[offset++]);
//    msg.ok = data[offset++] != 0;
//    uint8_t payloadCount = data[offset++];
//
//    for (int i = 0; i < payloadCount; ++i)
//    {
//        if (offset >= length) break; // 안전 체크
//
//        uint8_t typeByte = data[offset++];
//        InfoType type = static_cast<InfoType>(typeByte);
//
//        switch (type)
//        {
//        case InfoType::LS: {
//            LS::Message ls;
//            memcpy(&ls, data + offset, sizeof(LS::Message));
//            offset += sizeof(LS::Message);
//            msg.payload.emplace_back(ls);
//            break;
//        }
//        case InfoType::MFR: {
//            MFR::Message mfr;
//            memcpy(&mfr, data + offset, sizeof(MFR::Message));
//            offset += sizeof(MFR::Message);
//            msg.payload.emplace_back(mfr);
//            break;
//        }
//        case InfoType::LC: {
//            LC::Message lc;
//            memcpy(&lc, data + offset, sizeof(LC::Message));
//            offset += sizeof(LC::Message);
//            msg.payload.emplace_back(lc);
//            break;
//        }
//        case InfoType::Missile: {
//            Missile::Message missile;
//            memcpy(&missile, data + offset, sizeof(Missile::Message));
//            offset += sizeof(Missile::Message);
//            msg.payload.emplace_back(missile);
//            break;
//        }
//        case InfoType::Target: {
//            Target::Message target;
//
//            uint8_t targetCount = data[offset++];  // 개수 먼저 읽기
//            for (int t = 0; t < targetCount; ++t) {
//                Target::InfoPacket pkt;
//
//                memcpy(&pkt.id, data + offset, sizeof(pkt.id));
//                offset += sizeof(pkt.id);
//
//                memcpy(&pkt.position.x, data + offset, sizeof(pkt.position.x));
//                offset += sizeof(pkt.position.x);
//
//                memcpy(&pkt.position.y, data + offset, sizeof(pkt.position.y));
//                offset += sizeof(pkt.position.y);
//
//                memcpy(&pkt.z, data + offset, sizeof(pkt.z));
//                offset += sizeof(pkt.z);
//
//                memcpy(&pkt.speed, data + offset, sizeof(pkt.speed));
//                offset += sizeof(pkt.speed);
//
//                memcpy(&pkt.angle, data + offset, sizeof(pkt.angle));
//                offset += sizeof(pkt.angle);
//
//                memcpy(&pkt.shoot_down_time, data + offset, sizeof(pkt.shoot_down_time));
//                offset += sizeof(pkt.shoot_down_time);
//
//                target.data.push_back(pkt);
//            }
//
//            msg.payload.emplace_back(target);
//            break;
//        }
//        default:
//            // Unknown type, skip
//            break;
//        }
//    }
//
//    return msg;
//}
