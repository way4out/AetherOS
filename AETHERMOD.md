# AetherMod / AetherOS 8.2

AetherMod 8.2 is a local-first Nintendo DSi dual-screen cockpit.

## Pass 2

- VBlank-synchronized visual animation on both screens.
- Native DS palette/backdrop animation for lightweight immersive motion.
- Existing live telemetry bars continue to animate in the application frame loop.
- Navigation is silent; action feedback remains separate from scrolling/page movement.
- Home navigation exposes all 24 modules across three 8-module pages.
- D-pad LEFT/RIGHT and bottom-screen page area cycle pages; UP/DOWN select modules.
- Touch rows open modules; touch at the bottom page area advances the home page.
- AetherOS8.2.nds and AetherMod8.2.nds are equivalent build outputs.

The visual layer uses native libnds/DS hardware rather than claiming unsupported 24-bit display hardware.

## Hardware boundaries

The software does not claim that stock DSi hardware contains 5G, satellite, SDR, a physical QPU, or a holographic projector. Network/RF/TinySA functions remain gateway interfaces or simulations where external hardware is required.

## SD install

Copy the apps/AetherMod and data/AetherMod directories from the public bundle to the DSi SD card, then launch AetherOS8.2.nds.
