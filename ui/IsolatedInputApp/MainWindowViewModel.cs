using System.Collections.ObjectModel;
using System.Windows.Input;

namespace IsolatedInputApp;

public sealed class MainWindowViewModel
{
    public ObservableCollection<HidDeviceModel> HidDevices { get; } = new();
    public ObservableCollection<DevicePairModel> DevicePairs { get; } = new();

    public ICommand RefreshCommand { get; }
    public ICommand StartCommand { get; }
    public ICommand StopCommand { get; }

    public MainWindowViewModel()
    {
        RefreshCommand = new RelayCommand(_ => RefreshDevices());
        StartCommand = new RelayCommand(_ => StartIsolation());
        StopCommand = new RelayCommand(_ => StopIsolation());
    }

    private void RefreshDevices()
    {
        // Enumerate Win32_PnPEntity and SetupDi HID collection, then validate IDs against config.yml.
    }

    private void StartIsolation()
    {
        // Call service endpoint over named pipe, start assignment pipeline, and tail runtime logs.
    }

    private void StopIsolation()
    {
        // Call service endpoint to detach sessions and disable device isolation.
    }
}

public record HidDeviceModel(string Type, string DeviceInstanceId, int SessionId);
public record DevicePairModel(string KeyboardId, string MouseId, int SessionId);

public sealed class RelayCommand : ICommand
{
    private readonly Action<object?> _execute;

    public RelayCommand(Action<object?> execute) => _execute = execute;
    public event EventHandler? CanExecuteChanged;
    public bool CanExecute(object? parameter) => true;
    public void Execute(object? parameter) => _execute(parameter);
}
