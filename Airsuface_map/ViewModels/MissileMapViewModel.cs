using System.Collections.ObjectModel;
using System.ComponentModel;
using Airsuface_map.Models;
using System.Linq;

namespace Airsuface_map.ViewModels
{
    public class MissileMapViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<MockMissile> Missiles { get; } = new();

        // 최초 미사일 정보 리스트 (Id 기준)
        public ObservableCollection<MockMissile> FirstMissileInfos { get; } = new();

        public event Action<int, double, double>? OnMissileRemovedWithHit;

        public event PropertyChangedEventHandler? PropertyChanged;

        // 미사일 데이터 갱신 메서드 (Id 기준으로 업데이트)
        public void UpdateMissiles(IEnumerable<MockMissile> newMissiles)
        {
            foreach (var newMissile in newMissiles)
            {
                // 격추된 미사일은 추가도 갱신도 하지 않음
                if (newMissile.isHit)
                {
                    var existing = Missiles.FirstOrDefault(m => m.Id == newMissile.Id);
                    if (existing != null)
                    {
                        OnMissileRemovedWithHit?.Invoke(existing.Id, existing.X, existing.Y);

                        Missiles.Remove(existing);
                        var firstInfo = FirstMissileInfos.FirstOrDefault(f => f.Id == newMissile.Id);
                        if (firstInfo != null)
                            FirstMissileInfos.Remove(firstInfo);
                    }
                    continue;
                }

                var existingMissile = Missiles.FirstOrDefault(m => m.Id == newMissile.Id);
                if (existingMissile != null)
                {
                    existingMissile.X = newMissile.X;
                    existingMissile.Y = newMissile.Y;
                    existingMissile.Z = newMissile.Z;
                    existingMissile.Angle = newMissile.Angle;
                    existingMissile.Speed = newMissile.Speed;
                    existingMissile.isHit = newMissile.isHit;
                }
                else
                {
                    Missiles.Add(newMissile);
                    FirstMissileInfos.Add(new MockMissile
                    {
                        Id = newMissile.Id,
                        X = newMissile.X,
                        Y = newMissile.Y,
                        Z = newMissile.Z,
                        Angle = newMissile.Angle,
                        Speed = newMissile.Speed,
                        isHit = newMissile.isHit
                    });
                }
            }

            var toRemove = Missiles.Where(m => !newMissiles.Any(nm => nm.Id == m.Id)).ToList();
            foreach (var m in toRemove)
            {
                Missiles.Remove(m);
                var firstInfo = FirstMissileInfos.FirstOrDefault(f => f.Id == m.Id);
                if (firstInfo != null)
                    FirstMissileInfos.Remove(firstInfo);
            }
        }

    }
}