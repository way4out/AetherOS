#pragma once
#include <nds.h>

namespace aether::platform {

enum class Family : u8 {
    NDS = 0,
    DSi = 1,
    ThreeDS = 2,
    ModernNintendo = 3
};

struct Capabilities {
    Family family;
    bool dualScreen;
    bool touch;
    bool microphone;
    bool cameras;
    bool wifi;
    bool sd;
    bool extendedRuntime;
};

Capabilities detect();

// Build-target classification. An .nds build remains a DS-family executable;
// 3DS and later targets are integration targets requiring their native loader/runtime.
const char* integrationTarget();

}
