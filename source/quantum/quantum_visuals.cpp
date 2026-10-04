#include "quantum_visuals.h"
#include <stdio.h>
namespace aether::quantum::visuals {
static int clampBar(float p) {
  if (p < 0.0f) return 0;
  if (p > 1.0f) return 10;
  return (int)(p * 10.0f + 0.5f);
}
void draw(PrintConsole &top, PrintConsole &bottom, const Simulator &q,
          unsigned scanPassed, unsigned scanTotal, unsigned frame) {
  consoleSelect(&top);
  consoleClear();
  printf("\x1b[36;1mQUANTUM VISUAL FIELD\x1b[37;1m\n");
  printf("LIVE NODE RENDER / LOCAL DSi\n");
  printf("FRAME %lu   QUBITS %d   SHOTS %d\n", (unsigned long)frame, q.qubits, q.shots);
  printf("ALGORITHM %d   MEASURE %d\n\n", q.algorithm + 1, q.lastMeasurement);
  for (int i = 0; i < 8; ++i) {
    int bar = clampBar(q.probability[i]);
    printf("%c%02d |", (i == q.lastMeasurement) ? '>' : ' ', i);
    for (int k = 0; k < 10; ++k) printf("%c", k < bar ? '#' : '.');
    printf("| %0.3f\n", q.probability[i]);
  }
  consoleSelect(&bottom);
  consoleClear();
  printf("\x1b[35;1mQUANTUM NODE SCAN\x1b[37;1m\n\n");
  printf("NODE COHERENCE MATRIX\n");
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 16; ++x) {
      unsigned v = (unsigned)(x * 17 + y * 29 + frame * 3 + q.lastMeasurement * 11);
      char c = ((v % 19) < 4) ? '*' : ((v % 7) < 3 ? '+' : '.');
      printf("%c", c);
    }
    printf("\n");
  }
  printf("\nSCAN: %u/%u PASS\n", scanPassed, scanTotal);
  printf("A NEXT  X RESCAN  Y FIELD\n");
  printf("B RETURN   REAL-TIME SAFE RENDER\n");
}
}
