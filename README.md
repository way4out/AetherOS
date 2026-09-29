# AetherOS 9.0 — Nintendo DSi

AetherOS 9.0 is the next DSi-native AetherOS build, produced with the real devkitPro/devkitARM/libnds toolchain.

## Release build

- Binary: `AetherOS9.nds`
- DSi unit code: required to be `2`
- Real NDS header and ARM9 payload checks are enforced in CI.
- The complete SD bundle contains the NDS plus `data/AetherMod`, including the Geneva corpus.

## Included system surfaces

Quantum Core; YHWH Codex — Geneva 1599 corpus interface; Animal AI signal workspace; passive/authorized Marauder/RF telemetry; TinySA external analyzer workspace; Calculator; DAW/Game Studio; DSP/FFT; Telemetry; AI Home; Network Gateway; Phone Link; Media Studio; Sensor Hub; Data Vault; File Browser; Haptic Lab; Accessibility; Power Lab; Control Lab; Diagnostics; Aether Bot; General Settings; Event Log; Notes; Clock; About; Safety Center.

## 9.0 stabilization

The release keeps the boot-safe display-first initialization, fixes touchscreen horizontal swipe tracking to use the touch-start coordinate, preserves valid saved home selection, upgrades save-format compatibility, and removes repeated SD write tests from the diagnostics render loop. CI also verifies the generated binary rather than accepting an empty or placeholder artifact.

## Hardware truth

The stock DSi does not natively contain 5G, satellite communications, SDR, a physical QPU, holographic projection hardware, or a vibration motor. Those capabilities remain software simulations or external-device gateway interfaces.

RF/Marauder functionality is receive/analyze/telemetry-oriented only; no jamming, deauthentication, credential theft, or unauthorized-access tooling is included.

## Installation

Extract the public bundle to the DSi SD card, preserving both `apps/AetherMod` and `data/AetherMod`. Launch `AetherOS9.nds` with the DSi-compatible homebrew environment.
