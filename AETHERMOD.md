# AetherMod 8.0 — Pass/Age 3 / 7

Pass 3 is the integration pass: it expands the three-page 24-module cockpit and turns Pass 2 Phone Link into a runtime bridge surface.

## What changed
- Repaired the Pass 2 launcher regression: slot 24 now opens GENERAL SETTINGS.
- Preserved 24 modules across three pages.
- AI Safety, Family Safety and System remain consolidated under General Settings rather than occupying home slots.
- Added bounded cross-module runtime telemetry.
- Added phone session, companion, file-sync, telemetry and remote-control state.
- Quantum Core, YHWH Codex, Animal AI, RF, Network Gateway and Aether Bot now expose integration state.
- Kept local-first behavior and explicit hardware boundaries.

## Phone architecture
The supported architecture is:

**iPhone 15 or Android cellular backhaul -> Personal Hotspot -> DSi Wi-Fi -> AetherMod Network Gateway**

The phone supplies cellular connectivity; the stock DSi is not represented as containing a 5G modem. Nintendo documents DSi advanced Wi-Fi setup for compatible WPA networks, while Apple documents Personal Hotspot sharing over Wi-Fi.

## Phone Link Pass 3
- iPhone 15 profile and Android profile.
- Hotspot connect state.
- Session state and packet counters.
- RSSI and ping telemetry.
- Companion-link state.
- File-transfer readiness.
- Telemetry-stream readiness.
- Aether Bot remote-link readiness.
- Cross-module bus state.

These are runtime/gateway states; an actual phone-side companion service still requires a compatible web/PWA or external bridge.

## Module integration
- Quantum Core: phone telemetry bridge.
- YHWH Codex: corpus-sync bridge.
- Animal AI: sensor/phone bridge.
- Marauder/RF: authorized gateway state only.
- TinySA: external instrument gateway only.
- Calculator/DAW/DSP: local engines with cross-module routing indicators.
- Network Gateway: phone backhaul state.
- AI Home/Aether Bot: phone remote and Codex-link states.
- Telemetry: phone and cross-module counters.

## Hardware boundaries
No stock DSi hardware is claimed to be a physical 5G modem, SDR, QPU, satellite modem, TinySA instrument, or true animal-language translator. External hardware requires an actual compatible bridge.
