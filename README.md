# AetherCore 1 — Pass 7 / Nintendo DSi

AetherCore 1 Pass 7 is the current DSi production line for the AetherOS universal, gated runtime.

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

GitHub Actions builds a real DSi-compatible NDS using BlocksDS/libnds and validates the NDS header/payload before publishing:

- `AetherCore1.nds`
- `AetherOS-Pass7.nds`
- `AetherCore1-Pass7-DSi-Public.tar.gz`

Copy the published `AetherOS-Pass7.nds` to the DSi SD card. For the complete package, copy the included `apps/AetherMod` and `data/AetherMod` directories.

## Project

AetherOS is developed as a faith-inspired engineering project, with the stated aim of building for God.
