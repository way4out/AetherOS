# Universe Simulator+ Quantum Max Pass — 2026-10-04

This pass adds a deterministic, DSi-safe real-time Quantum Visual Field and a repeatable local quantum-node scan presentation.

## Rendering

- Uses the existing libnds text consoles and VBlank pacing.
- No private VBlank interrupt, no DMA ownership changes, and no camera state changes are introduced by the visual field.
- Probability bars render the simulator state directly.
- The node matrix is deterministic from frame, algorithm, and measurement state, so it can be replayed and validated without pretending it is a physical QPU.

## Capacity policy

A bootable DSi .nds is kept within standard cartridge-capacity encoding.
The production artifact is expanded to 512 MiB (536,870,912 bytes) with ROM capacity code 12. A request for a .nds larger than 2 GiB is intentionally not implemented because that would no longer be a valid, DSi-bootable artifact.

Large logical/runtime storage remains external/profile-based and is not claimed as physical DSi RAM or cartridge ROM.

## Verification

CI checks:
- exact 512 MiB artifact size
- DSi unit code 2
- ROM capacity code 12
- valid ARM9 offset/size bounds
- 79-module shell coverage
- Quantum Visual Field integration
- three-pass quantum self-test scan
- existing DSi capability and storage-profile checks

This pass does not claim physical-console execution testing; CI validates the binary structure and build reproducibility.
