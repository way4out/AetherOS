#pragma once
#include <stdint.h>
namespace aether::selfheal {
struct State { bool healthy; bool displaySafe; bool inputSafe; bool storageSafe; bool servicesSafe; uint16_t checks; uint16_t repairs; uint16_t faults; };
void init(State& state); void tick(State& state,uint32_t frame,bool safeMode); bool bootSafe(); const State& state();
}
