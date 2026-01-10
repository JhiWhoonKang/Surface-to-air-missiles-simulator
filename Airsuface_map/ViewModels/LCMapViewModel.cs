using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Airsuface_map.Models;

namespace Airsuface_map.ViewModels
{
    public class LCMapViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<LC> LCS { get; } = new();


        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public void UpdateLC(LC newLC)
        {
            if (newLC == null) return;

            LCS.Clear();
            LCS.Add(newLC);
        }
    }
}