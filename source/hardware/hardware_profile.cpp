#include "hardware_profile.h"
#include <fat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/camera.h>

namespace aether::hardware {

static bool gFatReady = false;
static bool gMicReady = false;
static bool gMicRunning = false;
static bool gCameraReady = false;
static bool gCameraSelected = false;

alignas(32) static s16 gMicBuffer[2048];

static void micCallback(void* completedBuffer, int length) {
    (void)completedBuffer;
    (void)length;
}

bool sdAvailable() {
    if (gFatReady) return true;
    gFatReady = fatInitDefault();
    return gFatReady;
}

static bool dirExists(const char* path) {
    struct stat st{};
    return stat(path, &st) == 0 && (st.st_mode & S_IFDIR);
}

static bool makeDir(const char* path) {
    if (dirExists(path)) return true;
    if (mkdir(path, 0777) == 0) return true;
    return dirExists(path);
}

bool ensureDirectories() {
    if (!sdAvailable()) return false;
    const char* dirs[] = {
        "REVF","REVF/CORE","REVF/QUANTUM","REVF/SOUND","REVF/DSP","REVF/LAB",
        "REVF/AI","REVF/NETWORK","REVF/PROJECTS","REVF/SAMPLES","REVF/PRESETS",
        "REVF/CIRCUITS","REVF/PLUGINS","REVF/CACHE","REVF/BENCH","REVF/LOGS","REVF/RECOVERY"
    };
    for (unsigned i=0;i<sizeof(dirs)/sizeof(dirs[0]);++i)
        if (!makeDir(dirs[i])) return false;
    return true;
}

bool writeBootMarker() {
    if (!sdAvailable() || !dirExists("REVF") || !dirExists("REVF/LOGS")) return false;
    FILE* f=fopen("REVF/LOGS/BOOT.LOG","w");
    if(!f) return false;
    fprintf(f,"AetherOS AetherCore1 DSi booted.\n");
    fclose(f);
    return true;
}

bool dsiMode() {
    return isDSiMode();
}

bool microphoneAvailable() {
    if(!dsiMode()) return false;
    if(!gMicReady) {
        soundEnable();
        gMicReady = true;
    }
    return gMicReady;
}

bool microphoneStart() {
    if(!microphoneAvailable()) return false;
    if(gMicRunning) return true;
    // Double-buffered PCM input; callback is intentionally lightweight.
    gMicRunning = soundMicRecord(gMicBuffer, sizeof(gMicBuffer), MicFormat_12Bit, 16000, micCallback) != 0;
    return gMicRunning;
}

void microphoneStop() {
    if(gMicRunning) soundMicOff();
    gMicRunning=false;
}

u16 microphoneLevel() {
    if(!gMicRunning) return 0;
    u32 sum=0;
    for(unsigned i=0;i<sizeof(gMicBuffer)/sizeof(gMicBuffer[0]);i+=32) {
        s32 v=gMicBuffer[i];
        if(v<0) v=-v;
        sum += (u16)(v>32767?32767:v);
    }
    return (u16)(sum / (sizeof(gMicBuffer)/sizeof(gMicBuffer[0])/32));
}

bool cameraAvailable() {
    if(!dsiMode()) return false;
    if(!gCameraReady) gCameraReady = cameraInit();
    return gCameraReady;
}

bool cameraSelectInner() {
    if(!cameraAvailable()) return false;
    gCameraSelected = cameraSelect(CAMERA_INNER);
    return gCameraSelected;
}

bool cameraSelectOuter() {
    if(!cameraAvailable()) return false;
    gCameraSelected = cameraSelect(CAMERA_OUTER);
    return gCameraSelected;
}

void cameraShutdown() {
    if(gCameraReady) cameraDeinit();
    gCameraReady=false;
    gCameraSelected=false;
}

void hardwareTick() {
    if(dsiMode()) {
        (void)microphoneAvailable();
        (void)cameraAvailable();
    }
}

}
