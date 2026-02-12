# MultiMK

Production-grade scaffold for a Windows multi-keyboard/multi-mouse isolation platform using a KMDF driver + Windows service + WPF UI.

## Repository structure

```text
MultiMK/
├─ .github/workflows/build-windows.yml
├─ config/
│  └─ config.yml
├─ docs/
│  ├─ architecture.md
│  └─ build.md
├─ driver/IsolatedInputDriver/
│  ├─ IsolatedInputDriver.c
│  ├─ IsolatedInputDriver.h
│  └─ IsolatedInputDriver.inf
├─ schemas/
│  └─ config.schema.yml
└─ ui/IsolatedInputApp/
   ├─ App.xaml
   ├─ App.xaml.cs
   ├─ IsolatedInputApp.csproj
   ├─ MainWindow.xaml
   ├─ MainWindow.xaml.cs
   └─ MainWindowViewModel.cs
```

## Core components

- **Kernel driver (KMDF filter skeleton)**: per-device session assignment and isolation toggles via IOCTL.
- **Windows service (C++)**: loads YAML config, validates mappings, configures driver, hosts named pipe control surface.
- **WPF frontend (.NET 8)**: HID discovery/pairing/session assignment/start-stop/status UX.

For design details, see [docs/architecture.md](docs/architecture.md).
For build/deploy details, see [docs/build.md](docs/build.md).
