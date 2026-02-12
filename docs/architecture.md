# MultiMK Production Architecture

## 1) End-to-end architecture (text diagram)

```text
+----------------------------------------------------------------------------------+
|                             IsolatedInputApp (WPF .NET 8)                       |
|  - HID discovery (SetupAPI / RawInput metadata)                                  |
|  - Pair keyboard + mouse                                                         |
|  - Session assignment and runtime controls                                       |
|  - Status, telemetry, audit log viewer                                           |
+-----------------------------------------+----------------------------------------+
                                          | Named Pipe / RPC (authenticated)
+-----------------------------------------v----------------------------------------+
|                   IsolatedInputService (Windows Service, C++)                   |
|  - Runs as LocalSystem; exposes restricted control endpoint                       |
|  - Loads/validates config.yml (schema validation + normalization)                |
|  - Maps Device Instance IDs to policy objects                                    |
|  - Sends IOCTL policy updates to kernel                                           |
|  - Monitors PnP arrival/removal + session lifecycle                               |
+-----------------------------------------+----------------------------------------+
                                          | IOCTL / Device Interface
+-----------------------------------------v----------------------------------------+
|           IsolatedInputDriver (KMDF lower-filter OR virtual HID miniport)       |
|  - Per-device context keyed by Device Instance ID                                 |
|  - Input packet classification and policy lookup                                  |
|  - Session/desktop routing rules                                                  |
|  - Suppression path for global queue injection                                    |
+-----------------------------------------+----------------------------------------+
                                          | HID stack interception / virtual report
+-----------------------------------------v----------------------------------------+
|                 Physical HID keyboards and mice (multiple sets)                  |
+----------------------------------------------------------------------------------+
```

## 2) Isolation model

- Use a per-device policy map in kernel keyed by normalized device instance ID.
- For each keyboard/mouse pair, define a target session (or desktop token) and enforce in dispatch path.
- Do not forward isolated packets to shared Win32 input sink.
- Represent each isolated endpoint as independent cursor/focus domain managed by assigned session shell.

## 3) Security model

- Service ACL must restrict control pipe to administrators and trusted UI SID.
- IOCTL validation must reject unknown device IDs, invalid session IDs, and unauthenticated caller context.
- Driver binaries must be signed (EV cert + attestation or HLK route for production).
- For development, enable testsigning and use test cert only on lab systems.
