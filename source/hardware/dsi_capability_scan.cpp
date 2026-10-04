#include "dsi_capability_scan.h"
#include "hardware_profile.h"
#include <nds.h>
namespace aether::hardware {
CapabilityScan scan(){
    CapabilityScan r{};
    r.dsi=dsiMode();
    r.touch=true;
    r.buttons=true;
    r.dualScreen=true;
    r.microphone=microphoneAvailable();
    r.camera=cameraAvailable();
    if(r.camera) cameraShutdown();
    r.sd=sdAvailable();
    r.audio=true;
    r.wifiBus=false;
    r.rumbleNative=false;
    unsigned n=0;
    n+=r.dsi?15:10; n+=r.touch?10:0; n+=r.buttons?10:0; n+=r.dualScreen?10:0;
    n+=r.microphone?10:0; n+=r.camera?10:0; n+=r.sd?15:0; n+=r.audio?10:0;
    r.score=n;
    return r;
}
}
