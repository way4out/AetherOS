#pragma once
#include <nds.h>

namespace aether::pass46 {
struct Health {
    u32 frames;
    u32 lastFrame;
    u32 stalledFrames;
    bool healthy;
};
void init(Health& health);
void tick(Health& health, u32 frame);
}
