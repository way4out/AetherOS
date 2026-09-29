#pragma once
#include <nds.h>
namespace aether::device {
struct Capability { bool wifi; bool bluetooth; bool camera; bool microphone; bool dsi; bool fiveGRelay; bool satelliteRelay; bool androidRelay; bool appleRelay; };
void init(); void tick(); Capability capabilities();
}
