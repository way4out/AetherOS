# AetherOS — AetherCore 5 / Nintendo DSi

AetherCore 7 is the current production integration line for the AetherOS DSi runtime.

## One-click build

The official build is produced by GitHub Actions from the repository source using BlocksDS/libnds. The pipeline validates the NDS header, DSi unit code, payload size, source integration gates, and complete SD bundle before publishing the artifact.

## AetherCore 7 integration

- Boot-safe display initialization before optional SD/FAT access.
- Storage-optional operation with deterministic safe mode.
- Dual-screen UI, DSi touch input, D-pad navigation, paging and scrolling.
- 79 addressable runtime/home systems, including a dedicated free-text Messaging surface.
- AetherCore 7 supervisor coordinating health, recovery, capability gating and offline operation.
- Existing universal platform/cross-generation/emulation fabrics remain capability-gated adapters rather than false claims of native foreign hardware.
- Self-heal hooks are bounded and deterministic.
- RF/security surfaces remain passive/authorized analysis and lab simulation only.
- External camera, microphone, network, TinySA, satellite, modern cellular, AI, QPU and projection capabilities are exposed only where compatible hardware/backend support actually exists.

## Memory and storage tier

- Retail Nintendo DSi main RAM is 16 MiB; a retail unit cannot safely provide 44 MiB of RAM to a homebrew application.
- AetherCore 7 does not fake a 44 MiB RAM allocation. The rollout provisions an SD-backed resource cache at REVF/CACHE/AETHER44.BIN while keeping the runtime RAM budget hardware-safe.

## Hardware truth

A stock Nintendo DSi cannot become modern Xbox, PlayStation, Apple, satellite, SDR, holographic or quantum hardware through software alone. AetherCore 7 therefore treats those systems as adapters/workspaces and keeps unsupported capabilities explicitly gated.

## Production package

The Actions artifact contains:
- AetherCore7.nds
- AetherOS-AetherCore7.nds
- complete SD deployment archive

Copy the NDS to the DSi SD card. For the complete deployment, extract the accompanying SD bundle and preserve its directory structure.

## Safety boundary

The project does not enable active RF interference, covert interception, credential theft, or unauthorized access. Security/RF functions are limited to passive telemetry, authorized workflows and local simulation.

## Status

CI verification is required for each public release artifact; this repository does not label an unbuilt or untested binary as verified.

## Human DSi instructions

See [AETHERCORE4_USER_GUIDE.md](AETHERCORE4_USER_GUIDE.md) for the complete touchscreen, button, camera, microphone, module, gateway, recovery, and 4 GB SD installation guide.

## AetherCore 5 release naming

The production workflow emits AetherCore5.nds and the AetherCore5 deployment artifact. RF interference/jamming and unauthorized interception remain disabled; interstellar/interdimensional surfaces are conceptual or external-gateway contracts only.

<!-- AetherCore5 CI release trigger -->


<!-- CI: AetherOS 5 direct-build validation -->

<!-- build validation pulse -->

<!-- checkout diagnostic pulse -->

<!-- docker diagnostic pulse -->

<!-- final build trigger -->

<!-- production build trigger -->

<!-- production build retry -->

<!-- docker cli test -->

<!-- production compile trigger -->

<!-- build job scheduling test -->

<!-- actual production build -->

<!-- docker smoke -->

<!-- docker volume smoke -->

<!-- blocksds image smoke -->

<!-- final real build trigger -->

<!-- isolated build script trigger -->

<!-- container build test -->

<!-- host docker build trigger -->

<!-- toolchain environment fix trigger -->

<!-- POSIX toolchain retry -->

<!-- supported shell retry -->

<!-- bash toolchain retry -->

<!-- explicit ARM compiler path trigger -->

<!-- arcade syntax fix build -->

<!-- verified artifact build -->

<!-- clean verified release trigger -->

<!-- canonical build trigger -->


## Messaging subsystem — AetherCore 7

The Messaging surface provides a local-first free-text composer, touchscreen keyboard, D-pad character selection, contact profiles, message queue accounting, send/receive test paths, and platform-aware contact accents for iPhone/Apple and Android profiles.

**Important interoperability boundary:** the DSi cannot natively impersonate Apple's iMessage or Android RCS/SMS carrier services. A real Internet delivery path requires an authorized, compatible gateway/service and any applicable carrier/service fees. AetherOS never claims that a local queued message was delivered unless the gateway reports delivery.

The UI uses a blue Apple/iPhone contact accent and green Android contact accent as **local contact metadata**. Actual bubble colors inside Apple Messages or an Android messaging client are controlled by those clients and are not settable by the DSi.

The subsystem is intentionally transport-neutral so the same composer can feed an authorized gateway without changing the on-device UI.
