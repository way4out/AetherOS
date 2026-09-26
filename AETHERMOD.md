# AetherMod 8.0 — Pass/Age 4 / 7

Pass 4 is the high-capacity systems pass: deeper analysis engines, richer live telemetry, stronger cross-module state and a more capable phone gateway.

## Pass 4 upgrades
- Quantum Core: coherence/phase/fidelity/entropy telemetry and measurement histogram.
- YHWH Codex: expanded canonical/index layers, searchable local SD corpus preview, name-layer/cross-reference/outlier routing.
- Animal AI: feature/confidence/event/history/output metrics. It is explicitly a signal classifier, not literal animal-language translation.
- Marauder/RF: rolling receive/analyze spectrum trace, RSSI/noise/SNR and authorized queue only. Jamming/deauth/credential capture remain disabled.
- TinySA Lab: rolling spectrum trace, marker, sweep, RBW, attenuation, peak and average metrics. External TinySA remains a physical gateway.
- Calculator: operation engine plus divide-by-zero guard and memory/entry telemetry.
- DAW: real 4x16 pattern state, track/mixer state and DSP bridge.
- DSP/FFT: bounded 32-sample analysis plus RMS, peak-bin and rolling history.
- Telemetry: Pass 4 bus, phone, quantum, animal, DSP, RF, TinySA and network metrics.
- Network Gateway: latency/health and phone-backhaul state.
- AI Home/Aether Bot: route/event telemetry and phone-remote state.
- Phone Link: pairing/ACK, byte counters and companion bridge state.

## Phone architecture
**iPhone 15 or Android cellular backhaul -> Personal Hotspot -> DSi Wi-Fi -> AetherMod Network Gateway.**

Apple documents Personal Hotspot sharing over Wi-Fi, Bluetooth and USB for supported client devices. For the DSi build, Wi-Fi is the primary transport. Nintendo documents DSi advanced Internet setup and WPA-capable connections. The DSi itself is not represented as a 5G modem.

## Data/corpus boundary
The Codex reader can search and preview data/AetherMod/codex.txt, but this repository does not claim that an entire religious corpus or every outlier text is bundled unless those source files are actually present on the SD card.

## RF/hardware boundary
RF/Marauder is receive/analyze/authorized-test UI only. TinySA, SDR, satellite, QPU and other external hardware require actual compatible bridges. No physical DSi hardware is claimed to have those capabilities.

## Release gate
Pass 4 is complete only after GitHub Actions produces a successful DSi build and rollout artifact.
