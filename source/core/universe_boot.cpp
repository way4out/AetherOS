#include "universe_boot.h"
#include "capacity_engine.h"
#include "diagnostics.h"
#include "recovery.h"
#include "self_heal.h"
#include "../hardware/hardware_profile.h"
#include "../storage/resource_store.h"
#include "../storage/module_registry.h"
#include "../quantum/quantum_core.h"
#include "../dsp/aether_dsp.h"
#include "../studio/aether_studio.h"
#include "../lab/aether_lab.h"
#include "../ai/aether_ai.h"
#include "../audio/aether_audio.h"

namespace {
bool gReady=false;
aether::selfheal::State gHealth{};
aether::quantum::Simulator gQuantum{};
}
namespace aether::boot {
void initialize(){
    gReady=false;
    recovery::init();
    diag::init();
    selfheal::init(gHealth);
    (void)hardware::ensureDirectories();
    (void)hardware::ensureStorageProfile();
    (void)storage::initialize();
    (void)storage::writeRegistry();
    capacity::init();
    quantum::init(gQuantum);
    dsp::init();
    studio::init();
    lab::init();
    ai::init();
    audio::init();
    gReady=true;
    selfheal::tick(gHealth,0,recovery::safeMode());
}
void tick(unsigned frame){
    if(!gReady) return;
    recovery::heartbeat();
    capacity::tick();
    diag::tick(frame);
    quantum::tick(gQuantum);
    dsp::tick();
    studio::tick();
    lab::tick();
    ai::tick();
    selfheal::tick(gHealth,frame,recovery::safeMode());
}
bool ready(){return gReady && selfheal::bootSafe();}
}
