#pragma once
#include <nds.h>
namespace aether::storage {
bool initialize();
bool writeManifest();
bool ensureCache();
bool verifyCache();
bool vaultReady();
u32 cacheMiB();
}
