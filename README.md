# AetherOS — AetherCore 4 / Nintendo DSi

AetherCore 4 is the current production integration line for the AetherOS DSi runtime.

## One-click build

The official build is produced by GitHub Actions from the repository source using BlocksDS/libnds. The pipeline validates the NDS header, DSi unit code, payload size, source integration gates, and complete SD bundle before publishing the artifact.

## AetherCore 4 integration

- Boot-safe display initialization before optional SD/FAT access.
- Storage-optional operation with deterministic safe mode.
- Dual-screen UI, DSi touch input, D-pad navigation, paging and scrolling.
- 78 addressable runtime/home systems.
- Core 2 supervisor coordinating health, recovery, capability gating and offline operation.
- Existing universal platform/cross-generation/emulation fabrics remain capability-gated adapters rather than false claims of native foreign hardware.
- Self-heal hooks are bounded and deterministic.
- RF/security surfaces remain passive/authorized analysis and lab simulation only.
- External camera, microphone, network, TinySA, satellite, modern cellular, AI, QPU and projection capabilities are exposed only where compatible hardware/backend support actually exists.

## Memory and storage tier

- Retail Nintendo DSi main RAM is 16 MiB; a retail unit cannot safely provide 44 MiB of RAM to a homebrew application.
- AetherCore 4 does not fake a 44 MiB RAM allocation. The rollout provisions an SD-backed resource cache at REVF/CACHE/AETHER44.BIN while keeping the runtime RAM budget hardware-safe.

## Hardware truth

A stock Nintendo DSi cannot become modern Xbox, PlayStation, Apple, satellite, SDR, holographic or quantum hardware through software alone. AetherCore 4 therefore treats those systems as adapters/workspaces and keeps unsupported capabilities explicitly gated.

## Production package

The Actions artifact contains:
- AetherCore4.nds
- AetherOS-AetherCore4.nds
- complete SD deployment archive

Copy the NDS to the DSi SD card. For the complete deployment, extract the accompanying SD bundle and preserve its directory structure.

## Safety boundary

The project does not enable active RF interference, covert interception, credential theft, or unauthorized access. Security/RF functions are limited to passive telemetry, authorized workflows and local simulation.

## Status

CI verification is required for each public release artifact; this repository does not label an unbuilt or untested binary as verified.

## Human DSi instructions

See [AETHERCORE4_USER_GUIDE.md](AETHERCORE4_USER_GUIDE.md) for the complete touchscreen, button, camera, microphone, module, gateway, recovery, and 4 GB SD installation guide.

## AetherCore 4 release naming

The production workflow now emits AetherCore4.nds and AetherOS-AetherCore4.nds. RF interference/jamming and unauthorized interception remain disabled; interstellar/interdimensional surfaces are conceptual or external-gateway contracts only.

<!-- AetherCore4 CI release trigger -->
