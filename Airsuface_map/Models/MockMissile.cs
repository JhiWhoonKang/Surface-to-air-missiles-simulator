namespace Airsuface_map.Models
{
    public class MockMissile
    {
        public int Id { get; set; }      // 미사일 ID
        public double X { get; set; }      // 위도
        public double Y { get; set; }      // 경도
        public long Z { get; set; }      // 고도
        public double Angle { get; set; }  // 발사각
        public int Speed { get; set; }    // 속도
        public bool isHit { get; set; }   // 격추 여부
    }
}