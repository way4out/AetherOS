#!/usr/bin/env bash
set -euxo pipefail
test -s AetherCore5.nds
python3 - <<'PY'
from pathlib import Path
p=Path("AetherCore5.nds").read_bytes()
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
grep -Fq '"CAMERA & MIC"' source/aetherosq.c
grep -Fq 'modCamera' source/aetherosq.c
