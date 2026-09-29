# AetherCore 1 — Pass 7 / Nintendo DSi

AetherCore 1 Pass 7 is the current DSi production line for the AetherOS universal, gated runtime.

## Latest DSi build

The latest successful GitHub Actions build is **run #756** from commit `88bd8c31f8e1850a7f82d42dd90bdb2313eb7960`.

**Download the real NDS package from the run artifacts:**
- [AetherOS Actions run #756](https://github.com/way4out/AetherOS/actions/runs/36628512880)
- Artifact: `AetherCore1-Pass6-DSi-Public`
- Included NDS files: `AetherCore1.nds` and `AetherOS-Pass6.nds`
- Artifact size: 1,837,511 bytes
- Artifact SHA-256: `8d06a764d235189b46bb4cd96be37d5c54e173e68f815fcb88736ab815814fd6`

The artifact is the actual output of the successful DSi build workflow; it is not a placeholder NDS.

## Pass 6 goals

- Deterministic DSi boot-safe startup with the display initialized before optional SD/FAT work.
- Dual-screen libnds UI with touch, D-pad navigation, scrolling, module paging, and safe-mode fallback.
- 77 addressable home systems: 29 core/runtime surfaces plus 48 executive systems.
- Universal platform fabric with explicit adapters for DSi/3DS/Wii/N64/Game Boy, later Nintendo, Sony legacy/modern, Xbox legacy/modern, Apple legacy/modern, Nokia legacy, and generic adapters.
- Emulator/frontend and web-gateway expansion points are capability-gated rather than pretending that one .nds binary can natively execute every foreign operating system.
- On-device rule-based diagnostics/self-heal hooks for boot/runtime faults; recovery remains deterministic and bounded on DSi hardware.
- RF/security surfaces are limited to passive/authorized observation and lab simulation. Active jamming, covert interception, credential theft, and unauthorized access remain locked.
- External capabilities such as modern cellular, satellite, SDR/TinySA, cloud AI, QPU hardware, and holographic projection are exposed as adapters/workspaces when hardware actually exists.

## Hardware truth

A stock Nintendo DSi cannot become an Xbox, PlayStation, Apple device, modern Nokia, SDR, satellite modem, or physical quantum computer through software alone. Pass 6 therefore uses a portable capability/adapter architecture: the DSi binary is the local control plane, while platform-specific backends can be added on devices that support them.

“Universal” means the architecture is designed to represent and route supported capabilities across generations; it does not claim access to secret, hidden, banned, or undocumented operating systems.

## RF/security boundary

The AetherOS security lab supports passive telemetry, authorized test workflows, and deterministic RF/network lab simulation. It does not transmit interference or provide covert interception. This keeps the DSi build suitable for lawful laboratory use while preserving an expansion interface for future, explicitly authorized hardware.

## Build artifact

GitHub Actions builds a real DSi-compatible NDS using BlocksDS/libnds and validates the NDS header/payload before publishing.

Current run #756 publishes:
- `AetherCore1.nds`
- `AetherOS-Pass6.nds`
- `AetherCore1-Pass6-DSi-Public.tar.gz`

Copy the published NDS file to the DSi SD card. For the complete package, use the accompanying archive from the same Actions run.

## Project

AetherOS is developed as a faith-inspired engineering project, with the stated aim of building for God.
