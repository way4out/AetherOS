#pragma once
#include <nds.h>
namespace aether::hardware {
struct CapabilityScan {
    bool dsi;
    bool touch;
    bool buttons;
    bool dualScreen;
    bool microphone;
    bool camera;
    bool sd;
    bool audio;
    bool wifiBus;
    bool rumbleNative;
    unsigned score;
};
CapabilityScan scan();
}
