# Build and Deployment Instructions

## Prerequisites

1. Windows 11 x64 development machine.
2. Visual Studio 2022 with Desktop C++, .NET Desktop, and WDK integration.
3. Windows Driver Kit matching target OS.
4. Administrator rights for driver install/service registration.

## Step-by-step

1. Clone repository.
2. Build driver in Release|x64 from WDK-enabled developer prompt.
3. Build service in Release|x64 (CMake/MSBuild).
4. Build WPF UI:
   - `dotnet publish ui/IsolatedInputApp/IsolatedInputApp.csproj -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true`
5. Generate `config/config.yml` (or edit the supplied template).
6. Install driver package (`pnputil /add-driver <inf> /install`).
7. Register service (`sc create MultiMKIsolatedInputSvc ...`).
8. Launch UI as administrator and start isolation.

## Test-signing workflow (development only)

1. `bcdedit /set testsigning on`
2. Reboot.
3. Sign `.sys` with lab cert and install cert to Trusted Root + Trusted Publishers.
4. Deploy and validate.
5. Disable when done: `bcdedit /set testsigning off`.

## Production signing guidance

- Prefer EV certificate + Microsoft attestation signing for distribution.
- For broader kernel trust and enterprise environments, complete HLK and WHQL workflows.
