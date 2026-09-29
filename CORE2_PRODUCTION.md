# AetherCore 2 — Production Integration

AetherCore 2 is the next integration layer for AetherOS. It supervises the existing DSi runtime, universal capability fabric, self-heal layer, storage-optional boot path, touch capability, and offline operation.

## Production contract
- Real Nintendo DSi `.nds` target remains the primary runtime.
- Display/UI initializes before optional SD/FAT work.
- SD absence is a supported degraded mode, not a blank-screen condition.
- Touch, storage, network, and external-device capabilities are capability-gated.
- Existing module implementations remain the source of truth; Core 2 coordinates them rather than replacing them with placeholders.
- Health/recovery is bounded and never claims unsupported DSi hardware exists.
- RF/security functions remain restricted to passive/authorized analysis and lab simulation.

## Integration surface
Core 2 is called once during `aether::init()` and once per frame from `aether::update()`. It does not own module actions, preventing duplicate input dispatch. The existing 77-module home/runtime surface remains addressable.

## Validation gate
The release workflow compiles with BlocksDS, validates the NDS header and size, runs source sanity checks, and publishes both the `.nds` and complete SD bundle.
