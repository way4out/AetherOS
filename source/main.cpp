#include <nds.h>
extern "C" int legacy_shell_main(void);
extern "C" void aether_universal_boot_guard(void);
int main(void){aether_universal_boot_guard();return legacy_shell_main();}
