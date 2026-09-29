# AetherCore1 Pass 4.6

Cumulative from the verified Pass 4.5 tree.

- Preserves the existing DSi hardware, camera/microphone, touch, module, web-fabric and device-fabric code paths.
- Resets transient input/service state on core initialization so a re-entry cannot inherit stale touch or deferred-service state.
- Adds a lightweight frame-progress health monitor for diagnostics without introducing a reset loop or blocking recovery path.
- CI verifies the Pass 4.6 runtime-health integration in addition to the existing real-NDS packaging checks.

The GitHub Actions workflow remains the source-to-NDS build path. A CI-produced `.nds` is a build artifact; physical DSi behavior still requires testing on actual hardware.
