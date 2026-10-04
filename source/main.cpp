#include <nds.h>
#include "core/universe_boot.h"
extern "C" int universe_frontend(void);
extern "C" void aether_universal_boot_guard(void);
int main(void){aether_universal_boot_guard();aether::boot::initialize();return universe_frontend();}
