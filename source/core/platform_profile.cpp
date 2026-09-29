#include "platform_profile.h"
#include "../hardware/hardware_profile.h"

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
    c.extendedRuntime = dsi;
    return c;
}

const char* integrationTarget() {
#if defined(AETHER_TARGET_3DS)
    return "Nintendo 3DS native integration";
#elif defined(AETHER_TARGET_WII)
    return "Nintendo Wii native integration";
#elif defined(AETHER_TARGET_N64)
    return "Nintendo 64 native integration";
#elif defined(AETHER_TARGET_GAMEBOY)
    return "Game Boy native integration";
#elif defined(AETHER_TARGET_MODERN_NINTENDO)
    return "Later Nintendo native integration";
#else
    return hardware::dsiMode() ? "Nintendo DSi/TWL .nds" : "Nintendo DS .nds";
#endif
}
}