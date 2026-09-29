#pragma once
#include <nds.h>

namespace aether::crossgen {

enum class Target : u8 {
    PlayStation1,
    PlayStation2,
    PlayStation3,
    PlayStation4,
    PlayStation5,
    PlayStation6,
    Xbox,
    Xbox360,
    XboxOne,
    XboxSeries
};

struct FabricState {
    Target target;
    bool gatewayReady;
    bool remoteOnly;
    bool authenticated;
    bool jamReady;
    u32 ticks;
};

void init(FabricState&);
void tick(FabricState&, u32 frame);
void cycle(FabricState&);
const char* name(Target);
const char* capability(Target);
bool targetAvailable(Target);
bool productionGateOpen(const FabricState&);

}
