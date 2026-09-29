#pragma once
#include <stdint.h>

namespace aether::universal {
enum class Target : uint8_t { NintendoDSi, Nintendo3DS, NintendoWii, NintendoN64, NintendoGameBoy, NintendoModern, SonyLegacy, SonyModern, XboxLegacy, XboxModern, AppleLegacy, AppleModern, NokiaLegacy, Generic };
enum class LinkMode : uint8_t { Local, AuthorizedRemote, EmulatorFrontend, WebGateway };
struct State { Target target; LinkMode mode; bool ready; bool authenticated; bool telemetryOnly; bool expansionGated; uint32_t heartbeats; uint32_t routeCount; uint32_t rejectedUnsafeOps; };
void init(State& state); void tick(State& state, uint32_t frame); void cycleTarget(State& state); void cycleMode(State& state); void authorize(State& state); bool routeAllowed(const State& state); const char* targetName(Target target); const char* modeName(LinkMode mode);
}
