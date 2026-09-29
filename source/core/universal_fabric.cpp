#include "universal_fabric.h"
namespace aether {
namespace universal {
namespace { State gState{}; }
void init(State& s){s.target=Target::NintendoDSi;s.mode=LinkMode::Local;s.ready=true;s.authenticated=false;s.telemetryOnly=true;s.expansionGated=true;s.heartbeats=0;s.routeCount=0;s.rejectedUnsafeOps=0;gState=s;}
void tick(State& s,uint32_t frame){++s.heartbeats;s.ready=true;s.telemetryOnly=true;s.expansionGated=true;if((frame&63u)==0u&&s.mode!=LinkMode::Local)++s.routeCount;gState=s;}
void cycleTarget(State& s){uint8_t n=static_cast<uint8_t>(s.target);const uint8_t count=static_cast<uint8_t>(Target::Generic)+1u;n=static_cast<uint8_t>((n+1u)%count);s.target=static_cast<Target>(n);s.authenticated=false;s.mode=LinkMode::Local;gState=s;}
void cycleMode(State& s){uint8_t n=static_cast<uint8_t>(s.mode);s.mode=static_cast<LinkMode>((n+1u)%4u);s.authenticated=false;gState=s;}
void authorize(State& s){s.authenticated=true;gState=s;}
bool routeAllowed(const State& s){return s.ready&&s.expansionGated&&(s.mode==LinkMode::Local||(s.mode!=LinkMode::Local&&s.authenticated));}
const char* targetName(Target t){switch(t){case Target::NintendoDSi:return "Nintendo DSi";case Target::Nintendo3DS:return "Nintendo 3DS";case Target::NintendoWii:return "Nintendo Wii";case Target::NintendoN64:return "Nintendo 64";case Target::NintendoGameBoy:return "Game Boy";case Target::NintendoModern:return "Later Nintendo";case Target::SonyLegacy:return "Sony legacy";case Target::SonyModern:return "Sony modern";case Target::XboxLegacy:return "Xbox legacy";case Target::XboxModern:return "Xbox modern";case Target::AppleLegacy:return "Apple legacy";case Target::AppleModern:return "Apple modern";case Target::NokiaLegacy:return "Nokia legacy";default:return "Generic adapter";}}
const char* modeName(LinkMode m){switch(m){case LinkMode::Local:return "LOCAL";case LinkMode::AuthorizedRemote:return "AUTHORIZED REMOTE";case LinkMode::EmulatorFrontend:return "EMULATOR FRONTEND";default:return "WEB GATEWAY";}}
}
}
