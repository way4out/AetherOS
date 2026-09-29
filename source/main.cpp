#include <nds.h>

/*
 * Production DSi entry point.
 *
 * The repository contains two UI/runtime layers.  The legacy_shell_main()
 * implementation is the complete 77-app DSi cockpit: it owns the boot-safe
 * display sequence, SD fallback, touch/page navigation, camera/microphone
 * integration, and the full application dispatcher.  The newer AetherCore
 * service layer remains compiled and available to modules/gateway code, but
 * the legacy shell is the stable hardware-facing launcher for this build.
 */
extern "C" int legacy_shell_main(void);

int main(void) {
    return legacy_shell_main();
}
