#include <nds.h>
#include <stdint.h>

/*
 * AetherMod 8.2 Pass 1 visual safety shim.
 *
 * Do not install a private VBlank IRQ here. The console renderer owns the
 * display timing and palette state. A second IRQ was able to overwrite the
 * console palette/mosaic state and produce a blank/white screen on boot.
 * Animation is driven by the synchronized main loop in aetherosq.c instead.
 */
void aetherVisualSafeFrame(uint32_t frame){
    (void)frame;
}
