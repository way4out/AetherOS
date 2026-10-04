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
// DSi retail RAM is 16 MiB; the 44 MiB rollout tier is therefore SD-backed storage, not RAM.
// This creates the cache container lazily so the runtime can use a larger resource budget safely.
bool ensure44MiBCache();
// Storage policy: profile a 500 GB SD media target with a runtime-dependent application target.
// These are policy values; actual accessibility is determined by the DSi/libfat runtime.
u64 sdMediaProfileBytes();
u64 sdUsableTargetBytes();
bool ensureStorageProfile();

}
