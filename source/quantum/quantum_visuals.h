#pragma once
#include <nds.h>
#include "quantum_core.h"
namespace aether::quantum::visuals {
void draw(PrintConsole &top, PrintConsole &bottom, const Simulator &q,
          unsigned scanPassed, unsigned scanTotal, unsigned frame);
}
