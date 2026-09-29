# AetherCore1 Pass 4.8 — Cross-Generation Integration Foundation

Pass 4.8 is cumulative from Pass 4.0 through Pass 4.7.

The production artifact remains a real AetherCore1.nds for Nintendo DS, DS Lite, DSi and DSi XL. DSi camera/microphone services remain capability-gated.

AetherCore now has an explicit integration boundary for Nintendo 3DS and later Nintendo systems. This means shared AetherCore service contracts can be carried into native targets; it does **not** claim that an ordinary .nds binary can execute natively on 3DS or later systems. Those systems require their native executable format, SDK/runtime, graphics/input/audio adapters and platform loader.

Compile-time hooks are provided through AETHER_TARGET_3DS and AETHER_TARGET_MODERN_NINTENDO. The DS-family build continues through the existing .nds toolchain.

CI verifies real ARM9 compilation/linking, NDS packaging, DSi unit code, Pass 4.5/4.6/4.7 gates, cross-generation target declarations and the complete SD bundle.

A successful CI build verifies compilation/package integrity; physical hardware testing is still required for each device model.