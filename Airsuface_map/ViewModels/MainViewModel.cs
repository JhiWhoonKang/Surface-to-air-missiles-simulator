using System;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using Airsuface_map.Models;

namespace Airsuface_map.ViewModels
{
    public class MainViewModel
    {

        bool first_init = false;
        private static double ConvertLongToDouble(long value)
        {
            // 8자리 소수부 분리
            long intPart = value / 10_000_000L;
            long fracPart = Math.Abs(value % 10_000_000L);
            double result = intPart + fracPart / 10_000_000.0;
            // 음수 처리
            if (value < 0 && intPart == 0)
                result *= -1;
            return result;
        }

        private static long ReadCustomInt64(BinaryReader reader)
        {
            // 8바이트 읽기
            byte[] originalBytes = reader.ReadBytes(8);
            if (originalBytes.Length < 8)
                throw new EndOfStreamException("Int64를 읽기에 충분한 바이트가 없습니다.");

            //Debug.WriteLine($"원본 바이트 배열: {BitConverter.ToString(originalBytes)}");

            // 2바이트씩 자리를 바꾸기 위해 2바이트 단위로 교환
            byte[] swappedBytes = new byte[8];
            swappedBytes[0] = originalBytes[7];
            swappedBytes[1] = originalBytes[6];
            swappedBytes[2] = originalBytes[5];
            swappedBytes[3] = originalBytes[4];
            swappedBytes[4] = originalBytes[3];
            swappedBytes[5] = originalBytes[2];
            swappedBytes[6] = originalBytes[1];
            swappedBytes[7] = originalBytes[0];

            //Debug.WriteLine($"오리지널 바이트 배열: {BitConverter.ToString(originalBytes)}");
            //Debug.WriteLine($"2바이트씩 자리 변경 후 바이트 배열: {BitConverter.ToString(swappedBytes)}");

            // 앞쪽의 0x00 바이트 제거
            int startIndex = 0;
            while (startIndex < swappedBytes.Length && swappedBytes[startIndex] == 0x00)
            {
                startIndex++;
            }

            // 모든 바이트가 0x00이면 0 반환
            if (startIndex == swappedBytes.Length)
            {
                return 0;
            }

            // 유의미한 바이트 배열 추출
            int significantLength = swappedBytes.Length - startIndex;
            byte[] significantBytes = new byte[significantLength];
            Array.Copy(swappedBytes, startIndex, significantBytes, 0, significantLength);

            // 바이트 배열을 뒤집기 (Big Endian에서 Little Endian으로)
            Array.Reverse(significantBytes);

            // 8바이트로 패딩 (필요한 경우)
            byte[] paddedBytes = new byte[8];
            Array.Copy(significantBytes, 0, paddedBytes, 0, significantBytes.Length);

            // unsigned long으로 변환
            ulong unsignedResult = BitConverter.ToUInt64(paddedBytes, 0);

            // 필요한 경우 long으로 캐스팅
            long result = (long)unsignedResult;

            return result;
        }

        public MissileMapViewModel MissileMapVM { get; } = new();
        public TargetMapViewModel TargetMapVM { get; } = new();
        public LCMapViewModel LCMapVM { get; } = new();
        public LSMapViewModel LSMapVM { get; } = new();
        public MFRMapViewModel MFRMapVM { get; } = new();

        // C++ 서버에서 온 바이너리 데이터 파싱 예시
        public void OnDataReceived(byte[] rawData)
        {
            if (rawData == null || rawData.Length < 8 || rawData.Length > 3072)
            {
                System.Diagnostics.Debug.WriteLine("[OnDataReceived] 데이터가 너무 짧거나 깁니다. 무시.");
                return;
            }

            // 1. 시작 바이트 확인 (0x51)
            if (rawData[0] != 0x51)
            {
                System.Diagnostics.Debug.WriteLine("[OnDataReceived] 시작 바이트가 0x51이 아님. 무시.");
                return;
            }

            // 2. payload length 추출
            int payloadLength = BitConverter.ToInt32(rawData, 1);
            int totalLength = 1 + 4 + payloadLength;
            if (rawData.Length < totalLength)
            {
                System.Diagnostics.Debug.WriteLine("[OnDataReceived] 전체 메시지가 아직 도착하지 않음. 무시.");
                return;
            }

            // 3. flag (payload 내 시작 3바이트)가 0x01 0x01 0x01인지 확인
            if (payloadLength < 3 || rawData[5] != 0x01 || rawData[6] != 0x01 || rawData[7] != 0x01)
            {
                System.Diagnostics.Debug.WriteLine("[OnDataReceived] radar/lc/ls flag가 유효하지 않음. 무시.");
                return;
            }

            // ↓↓↓ 아래부터는 기존 파싱 유지 ↓↓↓
            using var ms = new MemoryStream(rawData);
            using var reader = new BinaryReader(ms);

            // Header
            var commandType = reader.ReadByte(); // 0x51
            var length = reader.ReadInt32();     // payload length

            var numRadar = reader.ReadByte();    // 1
            var numLC = reader.ReadByte();       // 1
            var numLS = reader.ReadByte();       // 1
            var numTarget = reader.ReadByte();   // 최대 100
            var numMissile = reader.ReadByte();  // 1

            // MFR
            var mfr = new MFR {
                Id = reader.ReadInt32(),
                X = ConvertLongToDouble(ReadCustomInt64(reader)),
                Y = ConvertLongToDouble(ReadCustomInt64(reader)),
                Z = reader.ReadInt64(),
                Mode = reader.ReadByte(),
                Angle = reader.ReadDouble()
            };

            // LS
            var ls = new LS
            {
                Id = reader.ReadInt32(),
                X = ConvertLongToDouble(ReadCustomInt64(reader)),
                Y = ConvertLongToDouble(ReadCustomInt64(reader)),
                Z = reader.ReadInt64(),
                Mode = reader.ReadByte(),
                Angle = reader.ReadDouble()
            };

            // LC
            var lc = new LC {
                Id = reader.ReadInt32(),
                X = ConvertLongToDouble(ReadCustomInt64(reader)),
                Y = ConvertLongToDouble(ReadCustomInt64(reader)),
                Z = reader.ReadInt64()
            };

            // Missile
            MockMissile missile = null;
            if (numMissile > 0)
            {
                missile = new MockMissile
                {
                    Id = reader.ReadInt32(),
                    X = ConvertLongToDouble(reader.ReadInt64()),
                    Y = ConvertLongToDouble(reader.ReadInt64()),
                    Z = reader.ReadInt64(),
                    Speed = reader.ReadInt32(),
                    Angle = reader.ReadDouble()
                };
                reader.ReadInt64(); // detectTime
                reader.ReadInt64(); // interceptTime
                missile.isHit = reader.ReadBoolean();  // hit

                MissileMapVM.UpdateMissiles(new[] { missile });
            }
            else
            {
                MissileMapVM.UpdateMissiles(Array.Empty<MockMissile>());
            }

            // Targets
            var targets = new List<MockTarget>();
            for (int i = 0; i < numTarget; i++)
            {
                MockTarget target = new MockTarget();
                target.Id = reader.ReadInt32();
                target.X = ConvertLongToDouble(ReadCustomInt64(reader));
                target.Y = ConvertLongToDouble(ReadCustomInt64(reader));
                target.Z = ReadCustomInt64(reader);
                target.Speed = reader.ReadInt32();
                target.Angle = reader.ReadDouble();
                reader.ReadInt64(); // 상승각
                reader.ReadInt64(); // detectTime
                reader.ReadByte();  // priority
                target.isHit = reader.ReadBoolean();
                if(!target.isHit)
                    targets.Add(target);
            }

            TargetMapVM.UpdateTargets(targets);
            if (!first_init)
            {
                LCMapVM.UpdateLC(lc);
                LSMapVM.UpdateLS(ls);
                MFRMapVM.UpdateMFR(mfr);
                first_init = true;
            }
        }

    }
}