# AetherCore 4 — Nintendo DSi Human User Guide

## Scope
AetherCore 4 is a Nintendo DSi NDS application. It is designed to boot to a visible local interface first, operate without an SD card in safe mode, and expose supported hardware only when the DSi reports that capability.

A stock DSi does not gain 5G, satellite, interstellar/interdimensional transport, quantum hardware, modern cellular radios, or RF-jamming hardware from software. Those surfaces are gateway/workspace interfaces and remain capability-gated.

## Boot
1. Copy AetherCore4.nds to the SD card.
2. Launch it from the DSi loader.
3. The display initializes before optional SD/FAT access.
4. If storage is unavailable, the interface continues in safe mode.

## Home screen
- D-pad Up/Down: move through modules.
- D-pad Left/Right: change home page.
- A: open selected module.
- Touch: tap a large module row to open it.
- Swipe up/down: move between home pages.
- Lower touchscreen strip: advance to the next page.
- B: return home from a module.

## Universal touchscreen mapping
- Tap upper-left: B / Back
- Tap upper-right: Y
- Tap lower-left: X
- Tap lower-right: A
- Tap left/right edges: Left / Right
- Tap upper/lower center: Up / Down
- Swipe: directional navigation

Every module prints its keyboard/touch instructions on screen.

## DSi camera and microphone
- START: request a camera preview and save a preview image when the camera is available.
- SELECT: start/stop the DSi microphone when available.
- L+R: switch between inner and outer camera.
- Camera files are stored under data/AetherMod/camera/ when writable storage is available.
- The hardware status line reports camera state, preview count, capture count, microphone state, peak and RMS.

Camera operations can be unavailable on non-DSi hardware or if the camera rejects a transfer; the UI remains usable.

## Core modules
1. Quantum Core: local quantum-inspired simulator; measure, phase, reset and state navigation.
2. YHWH Codex: offline Geneva 1599 corpus reader when installed on SD.
3. Animal AI: local signal classification/vocalization workspace; not literal animal-language translation.
4. Passive Intercept Lab: receive-only/authorized metadata and telemetry workspace; payload/credential capture is not implemented.
5. TinySA Lab: spectrum-analysis workspace for compatible external hardware.
6. Calculator: arithmetic, memory, operation selection and divide-by-zero guard.
7. Jam Studio/Game Studio: local PSG music sequencer; audio feature, not RF jamming.
8. DSP/FFT: signal visualization and FFT controls.
9. Telemetry: runtime, storage, battery and module status.
10. AI Home: local deterministic workspace plus optional external gateway contract.
11. Network Gateway: DSi Wi-Fi and authorized gateway workspace.
12. Phone Link: authorized phone/gateway session workspace.
13. Media Studio: local media/player controls.
14. Sensor Hub: DSi-local and external-gateway sensor workspace.
15. Data Vault: SD data listing.
16. File Browser: read-only root navigation.
17. Haptic Lab: feedback level and sound controls.
18. Accessibility: scale, contrast and scrolling controls.
19. Power Lab: power profile, saver and brightness controls.
20. Control Lab: local control/event workspace.
21. Diagnostics: runtime and SD checks.
22. Aether Bot: bounded local automation workspace.
23. General Settings: AI, privacy, browser, downloads, wireless, sound and safety controls.
24. Event Log: event history.
25. Notes: local note storage.
26. Clock: live RTC display and 12/24-hour mode.
27. About: build/runtime information.
28. Safety Center: parental/content/wireless safety controls.
29. AetherCore: local systems/gameplay control.
30–77. Executive systems: mission, resources, security, communications, operations, development, knowledge, accessibility, finance, inventory, analytics, research, release, reliability and continuity workspaces.

## RF / jamming boundary
AetherCore 4 does not implement active RF interference, deauthentication, signal jamming, covert interception, credential capture or unauthorized network access.

RF surfaces are limited to passive/authorized analysis and local simulation. Jam Studio is an audio/PSG sequencer and does not transmit RF interference.

## Interstellar / interdimensional surfaces
These names can be used as conceptual workspaces or external gateway contracts. They do not claim that a stock DSi can physically create or traverse interstellar or interdimensional links.

## Recovery / no-black-screen design
The boot path initializes visible display output before storage-heavy work and has a storage-optional safe mode. CI validates that the resulting file is a real, non-empty NDS and checks the DSi unit-code/header gates.

A CI pass is build/structural verification, not proof that every physical DSi, SD card, loader or camera behaves identically. Real-device testing remains the final hardware gate.

## 4 GB SD target
The package is designed to fit comfortably within a 4 GB SD deployment. The NDS itself is much smaller than 4 GB; optional data such as the Geneva corpus, camera captures and user files live on the SD card.

## Public build
The GitHub Actions workflow produces:
- AetherCore4.nds
- AetherOS-AetherCore4.nds
- AetherCore4-DSi-Public.tar.gz

The archive contains the NDS plus the SD deployment structure and installation instructions.
