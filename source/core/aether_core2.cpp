#include "aether_core2.h"
#include "universal_fabric.h"
#include "self_heal.h"
namespace aether { namespace core2 { namespace { State g{}; universal::State fabric{}; selfheal::State heal{}; }
void init(State& s){ s.frames=0;s.faults=0;s.recoveries=0;s.moduleHeartbeats=0;s.health=100;s.initialized=true;s.safe=false;s.storageOptional=true;universal::init(fabric);selfheal::init(heal);g=s; }
void tick(State& s,uint32_t frame,bool sdReady,bool safeMode,bool touchReady,bool networkReady){ (void)networkReady; if(!s.initialized)init(s);++s.frames;++s.moduleHeartbeats;s.safe=safeMode;uint16_t h=100;if(!touchReady)h-=15;if(!sdReady)h-=5;if(safeMode)h-=5;if(h<70&&!safeMode){++s.faults;++s.recoveries;selfheal::tick(heal,frame,safeMode);h=70;}s.health=h;universal::tick(fabric,frame);if((frame&127u)==0u)selfheal::tick(heal,frame,s.safe);g=s; }
bool healthy(const State& s){return s.initialized&&s.health>=70&&s.recoveries<=s.faults;}
const char* status(const State& s){if(!s.initialized)return "BOOT";if(s.safe)return "SAFE";if(s.health>=90)return "NOMINAL";if(s.health>=70)return "DEGRADED";return "RECOVERY";}
} }
