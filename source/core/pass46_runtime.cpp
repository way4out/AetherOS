#include "pass46_runtime.h"

namespace aether::pass46 {
void init(Health& health) {
    health.frames = 0;
    health.lastFrame = 0;
    health.stalledFrames = 0;
    health.healthy = true;
}

void tick(Health& health, u32 frame) {
    ++health.frames;
    if (frame == health.lastFrame) {
        if (health.stalledFrames < 255) ++health.stalledFrames;
    } else {
        health.stalledFrames = 0;
        health.lastFrame = frame;
    }
    // A frame counter that stops changing is a diagnostic condition, not a reason
    // to reset the DSi or enter an unsafe recovery loop.
    health.healthy = health.stalledFrames < 8;
}
}
