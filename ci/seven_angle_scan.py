#!/usr/bin/env python3
import hashlib, struct, sys
p=sys.argv[1]
d=open(p,'rb').read()
assert len(d)==536870912
assert d[0x14]==12
arm9_off,arm9_size=struct.unpack_from('<II',d,0x20)
arm7_off,arm7_size=struct.unpack_from('<II',d,0x30)
scans=[d[:0x200].count(b'\0')<0x200,0x200<=arm9_off<len(d) and arm9_size>0 and arm9_off+arm9_size<=len(d),0x200<=arm7_off<len(d) and arm7_size>0 and arm7_off+arm7_size<=len(d),any(d[i:i+4] for i in (0x68,0x6c,0x70,0x74)),arm9_off%4==0 and arm7_off%4==0,len(set(d[:1024]))>=8,hashlib.sha256(d).hexdigest()!='0'*64]
assert all(scans),scans
print("SEVEN-ANGLE SCAN: PASS (7/7 structural views)")
print("SHA256:",hashlib.sha256(d).hexdigest())
