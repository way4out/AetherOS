# AetherOS Universal Fabric

AetherOS keeps the Nintendo DSi as the concrete runtime. Other generations and operating systems are represented by gated adapters, emulator frontends, or remote/web contracts; selecting a target never pretends unsupported hardware is locally executable.

Target families in the adapter contract include Nintendo generations, Sony, Xbox, Apple, Nokia/legacy mobile, and a generic extension point.

## RF boundary
The RF workspace is passive/authorized analysis only: local telemetry, spectrum-style simulation, and authorized lab metadata. It does not implement payload interception, credential capture, deauthentication, radio disruption, or RF jamming. The existing Jam Studio is local audio/game composition.

## Self-healing boundary
The boot guard establishes a deterministic 2D video baseline before the legacy 77-app shell. The self-heal component provides runtime invariants and safe-state recovery primitives. It is not a magical autonomous code-rewriter; arbitrary code repair remains a build/test operation.

## Expansion gate
Remote/emulator routes require explicit authorization state. This is the security boundary for cross-platform expansion.

## OS coverage
The architecture can catalog public operating systems and device families. It cannot truthfully claim access to secret, classified, hidden, or banned systems that are not publicly documented or available.
