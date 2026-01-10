// TransmitStructs.cpp
#include "TransmitStructs.h"
#include "systemstatus.h"

RadarStatus ConvertToRadarStatus(const MFRStatus& mfr) {
    RadarStatus r{};
    r.id = static_cast<uint8_t>(mfr.mfrId);
    r.mode = static_cast<uint8_t>(mfr.mode);  // enum class → 정수형
    r.angle = mfr.degree;
    r.position = mfr.position;
    return r;
}

LCStatus ConvertToLCStatus(const ::LCStatus& lc) {
    LCStatus out{};
    out.id = static_cast<uint8_t>(lc.LCId);
    out.position = lc.position;
    return out;
}

LSStatus ConvertToLSStatus(const ::LSStatus& ls) {
    LSStatus out{};
    out.id = static_cast<uint8_t>(ls.launchSystemId);
    out.mode = static_cast<uint8_t>(ls.mode);  // enum class → 정수형
    out.position = ls.position;
    out.angle = ls.launchAngle;
    return out;
}

MissileStatus ConvertToMissileStatus(const ::MissileStatus& m) {
    MissileStatus out{};
    out.id = static_cast<uint8_t>(m.missileId);
    out.position = m.position;
    out.degree = m.degree;
    out.locked = static_cast<uint8_t>(m.locked ? 1 : 0);  // bool → uint8_t
    return out;
}

TargetStatus ConvertToTargetStatus(const ::TargetStatus& t) {
    TargetStatus out{};
    out.id = static_cast<uint8_t>(t.targetId);
    out.position = t.position;
    out.degree = t.degree;
    out.speed = t.speed;
    return out;
}
