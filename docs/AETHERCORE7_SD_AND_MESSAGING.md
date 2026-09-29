# AetherCore 7 — 4 GB SD deployment and messaging

## SD capacity policy

AetherCore 7 is designed to run within the stock Nintendo DSi memory model. It does not attempt to fake or over-allocate physical RAM. The 4 GB SD target is a storage ceiling for the deployment bundle, not a RAM claim.

Recommended layout:

- `/AetherCore7.nds` — application
- `/data/AetherMod/` — configuration, save state, logs, message queue
- `/data/AetherMod/messaging/` — future gateway queue/cache
- `/data/AetherMod/Geneva/` — Geneva 1599 corpus
- `/data/AetherMod/cache/` — bounded resource cache
- `/data/AetherMod/media/` — user media
- `/data/AetherMod/projects/` — creative projects

The runtime remains storage-optional and boots to the UI even when SD initialization fails.

## Messaging

The Messaging surface supports:

- 160-character free-text composition
- touchscreen keyboard
- D-pad character selection
- backspace and clear behavior
- contact profiles
- Apple/iPhone and Android contact metadata
- local blue/green contact accents
- send queue accounting
- receive-test path
- transport-neutral gateway boundary

A stock DSi cannot directly provide iMessage, RCS, or carrier SMS service. Real delivery to an iPhone or Android phone requires an authorized gateway/service reachable over a supported network path. The device UI must not claim delivery until that gateway confirms it.

## RAM

Retail DSi main RAM is 16 MiB. AetherCore 7 uses the hardware-safe memory model and shifts large/static resources to SD-backed storage where appropriate. It does not claim 44 MiB of physical DSi RAM.

## Verification

The production GitHub Actions gate checks the NDS header, DSi unit code, ARM9 payload bounds, source integration, touchscreen home interaction, and Messaging subsystem integration before the artifact is published.
