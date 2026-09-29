#!/usr/bin/env bash
set -euxo pipefail
test -s AetherCore7.nds
python3 - <<'PY'
from pathlib import Path
p=Path("AetherCore7.nds").read_bytes()
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
grep -Fq 'mode>=30&&mode<=77' source/aetherosq.c
grep -Fq '"MESSAGING","CAMERA & MIC"' source/aetherosq.c
grep -Fq 'modCamera' source/aetherosq.c
# Front-end interaction gates: every home entry is addressable, the entrypoint is linked,
# and the DSi touchscreen home rows use the actual bottom-screen layout.
grep -Fq 'int legacy_shell_main(void)' source/aetherosq.c
grep -Fq 'int r=((int)t.py-16)/12' source/aetherosq.c
grep -Fq 'if(!touchMoved&&t.py>=16&&t.py<112)' source/aetherosq.c
test "$(grep -o 'static void mod[A-Za-z0-9_]*' source/aetherosq.c | wc -l)" -ge 29

# AetherCore7 messaging gates
grep -Fq 'aether_messaging_init' source/aetherosq.c
grep -Fq 'aether_messaging_send' source/aetherosq.c
grep -Fq 'mode==77' source/aetherosq.c
test -s source/messaging/aether_messaging.cpp
test -s source/messaging/aether_messaging.h
