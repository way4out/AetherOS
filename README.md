# AetherCore 1 — Pass 4.4 / Nintendo DSi

AetherCore 1 Pass 4.4 is the DSi hardware-integration stage of the AetherCore evolution.

## Pass 4.4

- 77 Home Screen systems remain addressable.
- All 48 executive systems use the common executive input path.
- DSi camera initialization and inner/outer camera selection use libnds.
- START captures a 256x192 camera preview and writes preview.ppm to SD.
- Full 640x480 camera capture writes capture_###.yuv to SD.
- SELECT starts/stops DSi microphone recording.
- Live microphone peak/RMS telemetry is surfaced throughout the module UI.
- L+R switches between the two DSi cameras.
- Camera/microphone initialization is DSi-gated.
- Existing touchscreen, scrolling, color, navigation, Geneva 1599 corpus, storage and executive gameplay layers remain integrated.

## Hardware truth

Retail DSi hardware provides two 640x480 cameras and a microphone. The implementation uses the real libnds camera transfer and microphone APIs rather than simulated peripheral values.

Stock DSi hardware does not provide 5G, satellite communications, SDR/TinySA hardware, a physical QPU, holographic projection, or a vibration motor; those remain software workspaces or external-device gateways.

## SD camera output

The application creates:
- fat:/data/AetherMod/camera/preview.ppm
- fat:/data/AetherMod/camera/capture_###.yuv

## Build

CI produces a real AetherCore1.nds with DSi header/payload checks and a complete public SD bundle.
