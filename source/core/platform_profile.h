#pragma once
#include <nds.h>
#include "crossgen_fabric.h"

namespace aether::platform {

enum class Family : u8 {
    NDS = 0,
    DSi = 1,
    ThreeDS = 2,
    Wii = 3,
    N64 = 4,
    GameBoy = 5,
    ModernNintendo = 6
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
    bool crossGenerationGateway;
    bool interstellarJamGateway;
};

Capabilities detect();
const char* integrationTarget();

}
