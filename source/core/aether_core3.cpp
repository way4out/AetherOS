#include "aether_core3.h"
#include <nds.h>
#include <fat.h>
#include <stdio.h>

namespace {
aether::core3::Health h{};
uint32_t tickCounter=0;
}

namespace aether::core3 {
uint32_t recommendedHeapHint(){
    // Retail DSi exposes 16 MiB main RAM. Keep static assets small and let
    // malloc-backed workspaces use the remaining heap instead of embedding
    // large graphics/data in the ARM9 image.
    return isDSiMode() ? (12u * 1024u * 1024u) : (2u * 1024u * 1024u);
}
void init(){
    h={};
    h.boots=1;
    h.dsiMode=isDSiMode();
    h.displayReady=true;
    h.inputReady=true;
    h.storageReady=fatInitDefault();
    h.heapHint=recommendedHeapHint();
    h.safe=true;
}
void tick(){
    ++tickCounter;
    h.ticks=tickCounter;
    if((tickCounter & 0x3FFu)==0){
        h.storageReady = h.storageReady || fatInitDefault();
    }
}
const Health& health(){ return h; }
}
