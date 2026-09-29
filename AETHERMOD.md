# AetherOS / AetherCore 1 — Pass 6

Pass 6 is the current DSi build and public package.

## DSi runtime

- Boot-safe dual-screen initialization happens before optional storage access.
- 77 systems remain addressable from the home deck.
- Touch, D-pad, paging, scrolling, and module actions are implemented in the DSi runtime.
- Camera and microphone support use DSi hardware APIs when available.
- Storage failure falls back to a visible safe mode instead of leaving the UI blank.

## Universal expansion fabric

Supported architecture targets include Nintendo DSi/3DS/Wii/N64/Game Boy, later Nintendo, Sony legacy/modern, Xbox legacy/modern, Apple legacy/modern, Nokia legacy, and generic adapters. Emulator frontend and web gateway modes are capability-gated.

A .nds file is a Nintendo DS-family executable. It cannot directly replace the native OS of an Xbox, PlayStation, Apple device, or 1990s Nokia. Those platforms require their own native frontend/backend adapters or emulators on capable hardware.

## Security and RF

PASSIVE RF, AUTHORIZED NET, LAB SIM, and GATEWAY HARDEN modes are exposed through the security lab. Active jamming and covert/unauthorized interception are intentionally locked; LAB SIM can model those behaviors without transmitting interference or accessing third-party communications.

## Self-heal

The runtime contains deterministic boot/runtime health guards. They are designed to keep the display/input path alive and isolate optional services when storage or external services fail.

## Public install

1. Download `AetherOS-Pass6.nds` for the direct DSi executable.
2. Copy it to the SD card.
3. Or copy the complete public package's `apps/AetherMod` and `data/AetherMod` directories.
4. Launch the NDS from the DSi menu/loader.

The public CI artifact is the source of truth for the binary; no placeholder NDS is published.
