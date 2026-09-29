#include "self_heal.h"
namespace aether::selfheal { namespace { State gState{}; uint32_t lastFrame=0; }
void init(State& s){s.healthy=true;s.displaySafe=true;s.inputSafe=true;s.storageSafe=true;s.servicesSafe=true;s.checks=0;s.repairs=0;s.faults=0;gState=s;lastFrame=0;}
void tick(State& s,uint32_t frame,bool safeMode){++s.checks;if(frame==0||frame<lastFrame){s.displaySafe=true;s.inputSafe=true;++s.repairs;}lastFrame=frame;if(safeMode)s.servicesSafe=true;s.healthy=s.displaySafe&&s.inputSafe&&s.storageSafe&&s.servicesSafe;if(!s.healthy)++s.faults;gState=s;}
bool bootSafe(){return gState.healthy&&gState.displaySafe&&gState.inputSafe;} const State& state(){return gState;}
}
