# AetherCore1 Pass 4.7 — Universal NDS/TWL Foundation

Pass 4.7 is cumulative from Pass 4.0 through Pass 4.6.

## Target
The shipped artifact remains a standard `.nds` executable intended for Nintendo DS-family hardware:
- Nintendo DS / DS Lite / DSi / DSi XL
- DSi-only services are capability-gated and are not required for boot.
- Dual-screen/touch behavior is retained.

## Hardware safety
Camera and microphone probing is now non-invasive during boot. DSi camera/microphone services are initialized only when explicitly requested by a module. This preserves a common runtime path for original NDS and DSi/TWL systems.

## Build contract
CI must:
1. compile and link the real AetherCore1 ELF;
2. package AetherCore1.nds;
3. validate the NDS header and DSi unit code;
4. validate Pass 4.6 runtime health;
5. validate Pass 4.7 platform detection;
6. publish the complete SD bundle artifact.

A successful CI build proves compilation and packaging. It does not constitute physical DSi/DS hardware testing.

## Scope boundary
A `.nds` binary is not a universal native executable for every Nintendo dual- or single-screen product. This build targets the Nintendo DS-family NDS/TWL execution environment; other Nintendo platforms require their own compatibility/runtime layer.
