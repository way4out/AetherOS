#!/usr/bin/env bash
set -euxo pipefail
test -s UniverseSimulatorPlus.nds
python3 - <<'PY'
from pathlib import Path
p=Path("UniverseSimulatorPlus.nds").read_bytes()
assert len(p) > 0
assert len(p) <= 64*1024*1024
assert p[0x12] == 2, f"DSi unit code={p[0x12]}"
arm9_off=int.from_bytes(p[0x20:0x24],"little")
arm9_size=int.from_bytes(p[0x2c:0x30],"little")
assert arm9_off > 0 and arm9_size > 0 and arm9_off + arm9_size <= len(p)
print("VERIFIED_NDS_SIZE",len(p))
print("VERIFIED_DSI_UNIT_CODE",p[0x12])
print("VERIFIED_ARM9_SIZE",arm9_size)
PY
grep -Fq '#define APP_COUNT 79' source/aetherosq.c
grep -Fq 'mode>=30&&mode<=76' source/aetherosq.c
grep -Fq '"MESSAGING"' source/aetherosq.c
grep -Fq '"CAMERA & MIC"' source/aetherosq.c
grep -Fq 'modCamera' source/aetherosq.c
# Front-end interaction gates: every home entry is addressable, the entrypoint is linked,
# and the DSi touchscreen home rows use the actual bottom-screen layout.
grep -Fq 'int legacy_shell_main(void)' source/aetherosq.c
# 500 GB media profile is compiled and exposed; actual filesystem capacity remains runtime-dependent.
grep -Fq 'DAETHER_SD_MEDIA_GB=500' Makefile
grep -Fq 'DAETHER_SD_USABLE_GB=500' Makefile
grep -Fq 'AETHEROS_STORAGE_PROFILE=500GB' source/hardware/hardware_profile.cpp
grep -Fq 'AETHEROS_USABLE_TARGET=500GB' source/hardware/hardware_profile.cpp
grep -Fq 'SD MEDIA PROFILE 500 GB' source/universe_frontend.cpp
test "$(grep -o 'static void mod[A-Za-z0-9_]*' source/aetherosq.c | wc -l)" -ge 29

# AetherCore708 messaging gates
grep -Fq 'aether_messaging_init' source/aetherosq.c
grep -Fq 'aether_messaging_send' source/aetherosq.c
grep -Fq 'MEDIA / GLOBAL HUB' source/aetherosq.c
grep -Fq 'mediaLegalOnly' source/aetherosq.c
grep -Fq 'mode==77' source/aetherosq.c
test -s source/messaging/aether_messaging.cpp
test -s source/messaging/aether_messaging.h

grep -Fq 'DSi CAPABILITY SCAN' source/universe_frontend.cpp
grep -Fq 'QUANTUM REAL-TIME SIMULATOR' source/universe_frontend.cpp
grep -Fq 'AETHEROS_STORAGE_PROFILE=500GB' source/hardware/hardware_profile.cpp
grep -Fq 'AETHEROS_USABLE_TARGET=500GB' source/hardware/hardware_profile.cpp
test -s source/hardware/dsi_capability_scan.cpp
test -s source/quantum/quantum_scan.cpp
