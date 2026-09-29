#pragma once
#include <nds.h>

namespace aether::platform {
enum class Family : u8 { NDS = 0, DSi = 1 };

struct Capabilities {
    Family family;
    bool dualScreen;
    bool touch;
    bool microphone;
    bool cameras;
    bool wifi;
    bool sd;
};

Capabilities detect();
}
