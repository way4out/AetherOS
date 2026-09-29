# AetherOS Pass 6 Release Manifest

Release line: AetherCore 1 / AetherOS 9.x
Target runtime: Nintendo DSi / NDS-compatible loaders
Build source: main
Release artifact: AetherOS-Pass6.nds

## Verified design scope

- DSi boot-safe display initialization before optional SD/FAT operations.
- 77 addressable systems.
- Touch/D-pad navigation and home paging.
- Universal capability fabric with explicit platform profiles and gated expansion.
- Emulator frontend and web gateway integration points.
- Deterministic runtime self-heal/health guards.
- Passive RF, authorized network testing, lab simulation, and gateway hardening.

## Platform model

The universal fabric is an adapter architecture. It does not claim that a DSi .nds binary can natively execute every historical or current OS. Each target needs a compatible native backend, emulator, or external gateway.

Undocumented, secret, or banned operating systems are not claimed as known or accessible.

## RF safety

No active radio jamming, covert interception, credential theft, or unauthorized access is implemented. Lab simulation can exercise defensive/test scenarios without transmitting interference or accessing third-party communications.

## Artifact integrity

The CI workflow must build the binary with BlocksDS/libnds, verify a non-empty NDS, verify DSi unit code 2, inspect the NDS header, run source sanity checks, and publish the direct .nds plus the complete SD bundle.
