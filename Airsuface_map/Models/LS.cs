namespace Airsuface_map.Models
{
    public class LS
    {
        public int Id { get; set; }
        // LS ID

        public double X { get; set; }
        // 위도 또는 X 좌표

        public double Y { get; set; }
        // 경도 또는 Y 좌표
        public long Z { get; set; }
        // 경도 또는 Z 좌표

        public byte Mode { get; set; }
        // LS 모드 정보

        public double Angle { get; set; }
        // LS 각도
    }
}