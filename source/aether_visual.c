#include <nds.h>
#include <stdint.h>

static volatile uint32_t aether_visual_frame = 0;

static inline uint16_t pulse_rgb(uint32_t f, uint32_t phase, int base){
    uint32_t t=(f+phase)&63u;
    int v=(t<32u)?(int)t:(int)(64u-t);
    int r=(base+v/4)&31, g=(base/2+v/3)&31, b=(base/3+v/2)&31;
    return RGB15(r,g,b);
}

static void aether_visual_vblank(void){
    uint32_t f=++aether_visual_frame;
    if((f&1u)!=0u) return;
    BG_PALETTE[0]=pulse_rgb(f,0,2);
    BG_PALETTE_SUB[0]=pulse_rgb(f,21,1);
    for(int i=0;i<16;i++){
        BG_PALETTE[i]=pulse_rgb(f,(uint32_t)(i*7),4+(i&3));
        BG_PALETTE_SUB[i]=pulse_rgb(f,(uint32_t)(31+i*5),3+(i&3));
    }
    REG_MOSAIC=(uint16_t)(((f>>5)&3u)|(((f>>5)&3u)<<4));
    REG_MOSAIC_SUB=REG_MOSAIC;
}

__attribute__((constructor))
static void aether_visual_install(void){
    irqSet(IRQ_VBLANK,aether_visual_vblank);
    irqEnable(IRQ_VBLANK);
}
