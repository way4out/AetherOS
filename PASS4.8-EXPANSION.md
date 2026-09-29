# AetherCore1 Pass 4.8 — Expanded Nintendo Integration

AetherCore1 Pass 4.8 now declares integration targets for Nintendo 3DS, Wii, Nintendo 64, Game Boy, and later Nintendo systems.

The production artifact remains an actual `.nds` executable for the DS-family hardware. The 3DS/Wii/N64/Game Boy/later targets are cross-platform integration boundaries and web-enabled expansion points; they are **not** claimed to execute those native platforms directly from an `.nds` file.

Native ports require their platform-specific executable format, SDK/toolchain, loader/runtime, graphics, audio and input adapters. AetherCore's shared service contracts are structured so those adapters can be added without changing the DS-family core contract.

Web expansion is an integration layer: network-connected catalogs/services can be exposed through AetherOS when an appropriate service endpoint and platform adapter exist. It does not turn a standard `.nds` into a universal Nintendo emulator.

Target hooks:
- `AETHER_TARGET_3DS`
- `AETHER_TARGET_WII`
- `AETHER_TARGET_N64`
- `AETHER_TARGET_GAMEBOY`
- `AETHER_TARGET_MODERN_NINTENDO`

The DS/DSi build continues to package as AetherCore1.nds with existing Pass 4.0–4.7 functionality and DSi camera/microphone capability gating. CI/build success proves source and package integrity; it does not substitute for physical testing on each platform.