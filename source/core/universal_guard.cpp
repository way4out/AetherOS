#include <nds.h>
#include "self_heal.h"
#include "universal_fabric.h"
extern "C" void aether_universal_boot_guard(){aether::selfheal::State h{};aether::selfheal::init(h);aether::universal::State u{};aether::universal::init(u);videoSetMode(MODE_0_2D);videoSetModeSub(MODE_0_2D);vramSetBankA(VRAM_A_MAIN_BG);vramSetBankC(VRAM_C_SUB_BG);swiWaitForVBlank();}
