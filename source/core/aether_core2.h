#pragma once
#include <nds.h>
#include <stdint.h>

namespace aether { namespace core2 {
struct State { uint32_t frames; uint32_t faults; uint32_t recoveries; uint32_t moduleHeartbeats; uint16_t health; bool initialized; bool safe; bool storageOptional; };
void init(State& state);
void tick(State& state, uint32_t frame, bool sdReady, bool safeMode, bool touchReady, bool networkReady);
bool healthy(const State& state);
const char* status(const State& state);
} }
