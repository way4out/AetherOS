#pragma once
#include "quantum_core.h"
namespace aether::quantum {
struct ScanReport {
    bool bell;
    bool grover;
    bool deutschJozsa;
    bool qft;
    bool teleport;
    bool measurement;
    unsigned passed;
    unsigned total;
};
ScanReport selfTest(Simulator&);\nScanReport tripleSelfTest(Simulator&);
}
