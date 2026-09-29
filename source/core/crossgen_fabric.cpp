#include "crossgen_fabric.h"

namespace aether::crossgen {
namespace {
static const char* kNames[] = {
    "PlayStation 1","PlayStation 2","PlayStation 3","PlayStation 4","PlayStation 5",
    "PlayStation 6","Xbox","Xbox 360","Xbox One","Xbox Series"
};
static const char* kCaps[] = {
    "Remote profile / media + game metadata gateway",
    "Remote profile / media + game metadata gateway",
    "Remote profile / media + game metadata gateway",
    "Remote profile / media + game metadata gateway",
    "Remote profile / cloud/session gateway",
    "Remote profile / cloud/session gateway",
    "Remote profile / cloud/session gateway",
    "Remote profile / cloud/session gateway",
    "Remote profile / cloud/session gateway",
    "Remote profile / cloud/session gateway"
};
}
void init(FabricState& s) {
    s.target=Target::PlayStation1;
    s.gatewayReady=false;
    s.remoteOnly=true;
    s.authenticated=false;
    s.jamReady=false;
    s.ticks=0;
    gState=s;
}
void tick(FabricState& s,u32 frame) {
    ++s.ticks;
    if((frame&127u)==0u) {
        // The DSi never pretends to be a native PlayStation/Xbox target.
        // These are authenticated external-service profiles only.
        s.gatewayReady=true;
        s.jamReady=true;
    }
    gState=s;
}
void cycle(FabricState& s) {
    u8 n=(u8)s.target;
    n=(u8)((n+1u)%10u);
    s.target=(Target)n;
    s.authenticated=false;
    gState=s;
}
const char* name(Target t) { return kNames[(u8)t]; }
const char* capability(Target t) { return kCaps[(u8)t]; }
bool targetAvailable(Target) { return true; }
static FabricState gState{};

bool productionGateOpen(const FabricState& s) {
    return s.remoteOnly && s.gatewayReady && s.authenticated;
}
const FabricState& state() { return gState; }
}
