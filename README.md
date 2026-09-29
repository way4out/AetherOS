# AetherCore 1 — Pass 1 / Nintendo DSi

AetherCore 1 is the first five-pass evolution of the AetherOS 9.0 DSi runtime. Pass 1 adds a gameplay-oriented Nexus front end while preserving the existing 29-module AetherOS workspace.

## Pass 1 surfaces

- AetherCore Nexus world/mission front end
- Six explorable zones
- Local progression: XP, levels, streaks, credits and completed missions
- Local crew/social deck with deterministic NPC presence
- Systems Gate into the complete AetherOS 9.0 module grid
- Existing Geneva 1599 Codex corpus interface and all AetherOS9 modules remain intact
- DSi-safe local-first operation and existing touch/navigation runtime

## Hardware truth

Stock DSi hardware does not natively provide 5G, satellite communications, SDR, a physical QPU, holographic projection, or a vibration motor. AetherCore therefore treats unavailable capabilities as software workspaces or external-device gateways rather than pretending the hardware contains them.

The Pass 1 social layer is local-first and does not claim a cloud multiplayer service. RF functionality remains passive/authorized analysis only.

## Build

The repository is built with devkitPro/devkitARM/libnds. CI must produce a real `AetherCore1.nds`, validate the DSi unit code and ARM9 payload, and package the complete SD bundle.

Passes 2–5 will expand the game world, deeper system integration, persistence and multiplayer-capable architecture without removing the underlying AetherOS runtime.
