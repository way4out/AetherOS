#include "pass49_emulation.h"

namespace aether::emulation {
static State gState{};

namespace {
static constexpr u16 kBudgetLocalUs = 15000;
static constexpr u16 kBudgetRemoteUs = 5000;
static constexpr u8 kQualityDefault = 2;
static constexpr u32 kDSiWorkRam = 12u * 1024u * 1024u;
static constexpr u32 kFrameBufferReserve = 2u * 256u * 192u * 2u;

static u8 renderClass(crossgen::Target t) {
    return static_cast<u8>(t);
}

static bool isRemoteTarget(crossgen::Target t) {
    return t != crossgen::Target::PlayStation1 &&
           t != crossgen::Target::PlayStation2;
}
}

void init(State& s) {
    s.target = crossgen::Target::PlayStation1;
    s.mode = Mode::LocalProfile;
    s.ready = true;
    s.authenticated = false;
    s.sessionLive = false;
    s.jamReady = false;
    s.frames = 0;
    s.packets = 0;
    s.dropped = 0;
    s.inputEpoch = 0;
    s.frameBudgetUs = kBudgetLocalUs;
    s.quality = kQualityDefault;
    s.workRamBudget = kDSiWorkRam;
    s.framebufferBudget = kFrameBufferReserve;
    s.renderEpoch = 0;
    s.renderedTarget = renderClass(s.target);
    s.rendererReady = true;
    gState = s;
}

void tick(State& s, u32 frame) {
    ++s.frames;

    if (isRemoteTarget(s.target) && s.mode == Mode::LocalProfile)
        s.mode = Mode::RemoteSession;

    s.frameBudgetUs = (s.mode == Mode::LocalProfile) ? kBudgetLocalUs : kBudgetRemoteUs;
    s.workRamBudget = kDSiWorkRam;
    s.framebufferBudget = kFrameBufferReserve;
    s.renderedTarget = renderClass(s.target);
    s.rendererReady = s.ready;
    if ((frame & 3u) == 0u) ++s.renderEpoch;

    /* Deterministic DSi-side session heartbeat. No fake console execution. */
    if ((frame & 63u) == 0u) {
        s.ready = true;
        if (s.mode != Mode::LocalProfile)
            s.jamReady = true;
    }

    if (s.dropped > s.packets)
        s.dropped = s.packets;

    gState = s;
}

void cycleTarget(State& s) {
    u8 n = static_cast<u8>(s.target);
    n = static_cast<u8>((n + 1u) % 11u);
    s.target = static_cast<crossgen::Target>(n);
    s.authenticated = false;
    s.sessionLive = false;
    s.jamReady = false;
    s.renderedTarget = renderClass(s.target);
    s.renderEpoch = 0;
    if (isRemoteTarget(s.target) && s.mode == Mode::LocalProfile)
        s.mode = Mode::RemoteSession;
    gState = s;
}

void cycleMode(State& s) {
    u8 n = static_cast<u8>(s.mode);
    n = static_cast<u8>((n + 1u) % 3u);
    s.mode = static_cast<Mode>(n);
    s.authenticated = false;
    s.sessionLive = false;
    s.jamReady = false;
    gState = s;
}

void submitInput(State& s, u16 keys, u16 touchX, u16 touchY) {
    (void)keys;
    (void)touchX;
    (void)touchY;
    ++s.inputEpoch;
    ++s.packets;
    if (s.mode == Mode::LocalProfile)
        s.sessionLive = true;
    gState = s;
}

void acknowledgeRemote(State& s) {
    if (s.mode == Mode::LocalProfile)
        return;
    s.authenticated = true;
    s.sessionLive = true;
    s.jamReady = true;
    gState = s;
}

const char* modeName(Mode m) {
    switch (m) {
    case Mode::LocalProfile: return "LOCAL PROFILE";
    case Mode::RemoteSession: return "REMOTE SESSION";
    default: return "STREAM FRONTEND";
    }
}

bool targetIsRemoteOnly(crossgen::Target t) {
    return isRemoteTarget(t);
}

bool productionReady(const State& s) {
    if (!s.ready) return false;
    if (!isRemoteTarget(s.target)) return s.sessionLive;
    return s.authenticated && s.sessionLive && s.jamReady;
}

const State& state() { return gState; }

}
