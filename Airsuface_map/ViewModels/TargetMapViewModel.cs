using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using Airsuface_map.Models;

namespace Airsuface_map.ViewModels
{
    public class TargetMapViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<MockTarget> Targets { get; } = new();

        public event PropertyChangedEventHandler? PropertyChanged;

        // 타겟 데이터 갱신 메서드 (Id 기준으로 업데이트)
        public void UpdateTargets(IEnumerable<MockTarget> newTargets)
        {
            foreach (var newTarget in newTargets)
            {
                var existing = Targets.FirstOrDefault(t => t.Id == newTarget.Id);
                if (existing != null)
                {
                    // 기존 타겟 정보 갱신
                    existing.X = newTarget.X;
                    existing.Y = newTarget.Y;
                    existing.Z = newTarget.Z;
                    existing.Angle = newTarget.Angle;
                    existing.Speed = newTarget.Speed;
                    existing.isHit = newTarget.isHit;
                }
                else
                {
                    // 새 타겟 추가
                    Targets.Add(newTarget);
                }
            }

            // 더 이상 존재하지 않는 타겟 제거
            var toRemove = Targets.Where(t => !newTargets.Any(nt => nt.Id == t.Id)).ToList();
            foreach (var t in toRemove)
                Targets.Remove(t);
        }
    }
}
