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
    public class LSMapViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<LS> LSS { get; } = new();

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([System.Runtime.CompilerServices.CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public void UpdateLS(LS newLS)
        {
            if (newLS == null) return;

            LSS.Clear();
            LSS.Add(newLS);
        }
    }
}
