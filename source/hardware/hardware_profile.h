#pragma once
#include <nds.h>

namespace aether::hardware {
bool sdAvailable();
bool ensureDirectories();
bool writeBootMarker();

// DSi/TWL hardware services.
bool dsiMode();
bool microphoneAvailable();
bool microphoneStart();
void microphoneStop();
u16 microphoneLevel();
bool cameraAvailable();
bool cameraSelectInner();
bool cameraSelectOuter();
void cameraShutdown();
void hardwareTick();
}
