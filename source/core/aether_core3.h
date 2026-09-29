#pragma once
#include <stdint.h>

namespace aether::core3 {
struct Health {
    uint32_t boots;
    uint32_t ticks;
    uint32_t faults;
    uint32_t repairs;
    uint32_t heapHint;
    bool dsiMode;
    bool storageReady;
    bool displayReady;
    bool inputReady;
    bool safe;
};

void init();
void tick();
const Health& health();
uint32_t recommendedHeapHint();
}
