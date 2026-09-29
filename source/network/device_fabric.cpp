#include "device_fabric.h"
#include "../hardware/hardware_profile.h"
#include "network_fabric.h"
#include "../radio/radio_gateway.h"
namespace { aether::device::Capability c={}; }
namespace aether::device {
void init(){
 c.wifi=network::status(network::LINK_WIFI).configured;
 c.bluetooth=radio::capabilities().bluetooth;
 c.camera=hardware::cameraAvailable();
 c.microphone=hardware::microphoneAvailable();
 c.dsi=hardware::dsiMode();
 c.fiveGRelay=radio::capabilities().fiveG;
 c.satelliteRelay=radio::capabilities().satellite;
 c.androidRelay=false; c.appleRelay=false;
}
void tick(){c.wifi=network::status(network::LINK_WIFI).available;}
Capability capabilities(){return c;}
}
