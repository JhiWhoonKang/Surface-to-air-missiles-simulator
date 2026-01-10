using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Airsuface_map.Models;

namespace Airsuface_map.ViewModels
{
    public class MFRMapViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<MFR> MFRS { get; } = new();

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([System.Runtime.CompilerServices.CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public void UpdateMFR(MFR newMFR)
        {
            if (newMFR == null) return;

            MFRS.Clear();
            MFRS.Add(newMFR);
        }
    }
}
