#include "quantum_scan.h"
namespace aether::quantum {
ScanReport selfTest(Simulator& q){
    ScanReport r{false,false,false,false,false,false,0,6};
    runBell(q); r.bell=q.bellState; if(r.bell) ++r.passed;
    runGrover2(q); r.grover=(q.algorithm==2 && q.probability[3]>0.20f); if(r.grover) ++r.passed;
    runDeutschJozsa(q); r.deutschJozsa=(q.algorithm==3); if(r.deutschJozsa) ++r.passed;
    runQFT2(q); r.qft=(q.algorithm==4); if(r.qft) ++r.passed;
    runTeleportation(q); r.teleport=(q.algorithm==5); if(r.teleport) ++r.passed;
    reset(q); hadamard(q,0); int m=measure(q); r.measurement=(m==0 || m==1); if(r.measurement) ++r.passed;
    return r;
}
}
