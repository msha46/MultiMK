#include <windows.h>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

constexpr wchar_t kServiceName[] = L"MultiMKIsolatedInputSvc";
constexpr wchar_t kDriverSymbolicLink[] = L"\\\\.\\MultiMKIsolatedInput";

class DriverChannel {
public:
    bool Connect() {
        driverHandle_ = CreateFileW(kDriverSymbolicLink, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        return driverHandle_ != INVALID_HANDLE_VALUE;
    }

    bool AssignSession(const std::wstring& deviceInstanceId, ULONG sessionId) {
        struct SessionAssignment {
            wchar_t DeviceInstanceId[260];
            ULONG SessionId;
        } payload{};

        wcsncpy_s(payload.DeviceInstanceId, deviceInstanceId.c_str(), _TRUNCATE);
        payload.SessionId = sessionId;

        DWORD bytesReturned = 0;
        return DeviceIoControl(driverHandle_, 0x80502008, &payload, sizeof(payload), nullptr, 0, &bytesReturned, nullptr) == TRUE;
    }

    ~DriverChannel() {
        if (driverHandle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(driverHandle_);
        }
    }

private:
    HANDLE driverHandle_{ INVALID_HANDLE_VALUE };
};

class ConfigManager {
public:
    bool Load(const std::wstring& path) {
        config_ = YAML::LoadFile(std::string(path.begin(), path.end()));
        return config_["devices"] && config_["settings"];
    }

    std::vector<std::pair<std::wstring, ULONG>> BuildSessionAssignments() const {
        std::vector<std::pair<std::wstring, ULONG>> assignments;
        for (const auto& device : config_["devices"]) {
            auto keyboard = device["keyboard"].as<std::string>();
            auto mouse = device["mouse"].as<std::string>();
            auto session = device["session"].as<ULONG>();

            assignments.emplace_back(std::wstring(keyboard.begin(), keyboard.end()), session);
            assignments.emplace_back(std::wstring(mouse.begin(), mouse.end()), session);
        }
        return assignments;
    }

private:
    YAML::Node config_;
};

int wmain(int argc, wchar_t** argv)
{
    std::wstring configPath = argc > 1 ? argv[1] : L"config\\config.yml";

    ConfigManager config;
    if (!config.Load(configPath)) {
        return ERROR_BAD_CONFIGURATION;
    }

    DriverChannel channel;
    if (!channel.Connect()) {
        return GetLastError();
    }

    for (const auto& [deviceId, sessionId] : config.BuildSessionAssignments()) {
        channel.AssignSession(deviceId, sessionId);
    }

    return ERROR_SUCCESS;
}
