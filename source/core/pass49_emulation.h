#pragma once
#include <nds.h>
#include "crossgen_fabric.h"

namespace aether::emulation {

/*
 * Pass 4.9 is a DSi-side emulation/gateway facade.
 * It never claims the DSi can execute modern console binaries natively.
 * Targets beyond the DSi's practical CPU/GPU envelope are represented by
 * deterministic input/state adapters and an authenticated remote-session path.
 */
enum class Mode : u8 {
    LocalProfile = 0,
    RemoteSession = 1,
    StreamFrontend = 2
};

struct State {
    crossgen::Target target;
    Mode mode;
    bool ready;
    bool authenticated;
    bool sessionLive;
    bool jamReady;
    u32 frames;
    u32 packets;
    u32 dropped;
    u32 inputEpoch;
    u16 frameBudgetUs;
    u8 quality;
};

void init(State&);
void tick(State&, u32 frame);
void cycleTarget(State&);
void cycleMode(State&);
void submitInput(State&, u16 keys, u16 touchX, u16 touchY);
void acknowledgeRemote(State&);
const char* modeName(Mode);
bool targetIsRemoteOnly(crossgen::Target);
bool productionReady(const State&);
const State& state();

}
