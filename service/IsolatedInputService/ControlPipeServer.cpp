#include <windows.h>
#include <string>

constexpr wchar_t kControlPipe[] = L"\\\\.\\pipe\\MultiMK.Control";

void RunControlPipeServer()
{
    HANDLE pipe = CreateNamedPipeW(
        kControlPipe,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096,
        4096,
        0,
        nullptr);

    if (pipe == INVALID_HANDLE_VALUE) {
        return;
    }

    while (true) {
        if (!ConnectNamedPipe(pipe, nullptr) && GetLastError() != ERROR_PIPE_CONNECTED) {
            break;
        }

        char buffer[2048]{};
        DWORD bytesRead = 0;
        if (ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, nullptr)) {
            // Parse command, validate ACL/session ownership, issue IOCTL to driver.
        }

        DisconnectNamedPipe(pipe);
    }

    CloseHandle(pipe);
}
