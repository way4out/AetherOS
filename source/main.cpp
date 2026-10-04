#include <nds.h>
extern "C" int universe_frontend(void);
extern "C" void aether_universal_boot_guard(void);
int main(void){aether_universal_boot_guard();return universe_frontend();}
