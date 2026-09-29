#include "platform_profile.h"
#include "hardware_profile.h"

namespace aether::platform {
Capabilities detect() {
    const bool dsi = hardware::dsiMode();
    Capabilities c{};
    c.family = dsi ? Family::DSi : Family::NDS;
    c.dualScreen = true;
    c.touch = true;
    c.microphone = dsi;
    c.cameras = dsi;
    c.wifi = true;
    c.sd = hardware::sdAvailable();
    return c;
}
}
