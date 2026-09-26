# AetherMod 8.0 — Final DSi Public Release

A Nintendo DSi-native local-first cockpit and homebrew systems platform.

## Final release

**AetherMod 8.0 — Pass 7 / 7**  
**Status:** CI-verified public rollout  
**Final identity commit:** 737d9ffa9101841c1a20bcceef4d84d4703170a0  
**Artifact:** 282,461 bytes  
**SHA-256:** 132624e41c67dccd1430e4e7d28a02742248c0aade53298ca108bf4c46cb5e40

The downloadable public build is published through the repository's GitHub Actions release artifact.

## Final system

- Two-screen color cockpit
- Touchscreen + physical controls
- Speaker/audio feedback
- Microphone and camera hardware pathways
- Indicator-LED state pathway
- Live animated visual system
- Quantum Core software simulation/telemetry
- YHWH Codex local SD corpus interface
- Animal AI signal/classifier interface
- Marauder/RF receive/analyze/telemetry interface
- TinySA Lab external analyzer bridge
- Calculator
- DAW Studio
- DSP/FFT
- Telemetry master bus
- AI Home + Aether Bot routing
- Network Gateway
- Phone Link
- Media Studio
- Sensor Hub
- Data Vault
- File Browser
- Haptic Lab
- Accessibility
- Power Lab
- Control Lab
- Diagnostics
- General Settings

## Hardware truth

The stock Nintendo DSi does not contain native 5G, satellite communications, Bluetooth, SDR, a physical QPU, holographic projection hardware, or a vibration motor. AetherMod exposes those capabilities as software simulations, telemetry surfaces, or external-device gateway interfaces rather than claiming native hardware.

For cellular connectivity, the supported architecture is **phone 5G backhaul → phone Wi-Fi hotspot → DSi Wi-Fi → AetherMod Network Gateway**.

Camera and microphone integration is represented through hardware capability/pathway states; this build does not claim synthetic telemetry is equivalent to captured sensor samples.

## RF safety

RF/Marauder functions are limited to receive/analyze/telemetry-oriented workflows and authorized external hardware. No jamming, deauthentication, credential theft, or unauthorized-access tooling is included.

## Build

GitHub Actions uses devkitPro/devkitARM to build the Nintendo DSi NDS application and package the SD workspace.

Repository: https://github.com/way4out/AetherOS-Rev-F

## Public rollout

1. Download the latest successful GitHub Actions artifact.
2. Extract the package to the intended DSi SD-card workspace.
3. Use the generated NDS application with the appropriate DSi homebrew environment.
4. Preserve the packaged data/AetherMod directory so Codex, Animal, and configuration resources remain available.

## Development

The final Pass 7 source remains public in this repository. Earlier Pass 1–6 history is retained through Git commits and CI artifacts.