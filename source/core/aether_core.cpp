#include "aether_core.h"
#include "../i18n/aether_i18n.h"
#include "../benchmark/benchmark.h"
#include "../hardware/hardware_profile.h"
#include "../ui/aether_ui.h"
#include "../quantum/quantum_core.h"
#include "../engine/engine_modules.h"
#include "../audio/aether_audio.h"
#include "../dsp/aether_dsp.h"
#include "../lab/aether_lab.h"
#include "../ai/aether_ai.h"
#include "../network/network_fabric.h"
#include "../radio/radio_gateway.h"
#include "../network/gateway_session.h"
#include "../network/gateway_security.h"
#include "../network/gateway_manager.h"
#include "../compute/remote_compute.h"
#include "system_graph.h"
#include "recovery.h"
#include "governor.h"
#include "diagnostics.h"
#include "hil.h"
#include "mission_control.h"
#include "capacity_engine.h"
#include "../studio/aether_studio.h"
#include "../settings/aether_settings.h"
#include "../theme/aether_theme.h"
#include "../security/aether_security_lab.h"
#include "../animal/aether_animal.h"
#include "../codex/aether_yhwh_codex.h"
#include "../harmonic/aether_prime_harmonic.h"
#include "../heritage/aether_heritage.h"
#include "../os/aether_os_fabric.h"

namespace { aether::quantum::Simulator q; bool servicesStarted=false; bool touchWasDown=false; bool touchGestureConsumed=false; int touchStartX=-1,touchStartY=-1; }

namespace aether {
void init(SystemState&s){
    s={false,false,false,false,false,false,false,false,0,0,0,false,false,0,0,0,0,0,0,0,0,0,0,0};
    videoSetMode(MODE_0_2D); videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG); vramSetBankC(VRAM_C_SUB_BG);
    consoleDemoInit(); consoleClear(); s.touchReady=true;
    quantum::init(q); engine::init(); graph::init(); recovery::init(); governor::init(); diag::init(); hil::init(); mission::init(); capacity::init();
    studio::init(); gate::init(); compute::init(); settings::init(); i18n::init(); codex::init(); harmonic::init(); osfabric::init(); i18n::adjust((int)settings::current().language-1); theme::init(); securitylab::init(); animal::init(); heritage::init(); ui::init();
}
static void startDeferredServices(SystemState&s){
    if(servicesStarted) return;
    servicesStarted=true;
    s.sdReady=hardware::sdAvailable();
    if(s.sdReady) { s.sdWriteReady=hardware::ensureDirectories()&&hardware::writeBootMarker(); if(s.sdWriteReady) settings::load(); }
    benchmark::runQuick(s.benchmarkComplete); radio::init(); session::init(); securitylab::init();
    s.quantumReady=true; s.audioReady=audio::init(); dsp::init(); lab::init(); ai::init(); studio::init(); network::init();
    s.networkReady=network::status(network::LINK_WIFI).available; s.gatewayConfigured=radio::configured(); s.projectSaved=engine::projectExists();
}
static void touchHome(SystemState&s,touchPosition&t){
    if(t.px>=10 && t.px<254 && t.py>=58 && t.py<178){
        const int col=(t.px-10)/61, row=(t.py-58)/30;
        if(col<4 && row<4){ const int m=row*4+col; if(m>=0 && m<MOD_COUNT){ s.selectedModule=m; s.screen=m+1; } }
    }
}
static void doAction(SystemState&s){
    switch(s.selectedModule){
    case MOD_QUANTUM: quantum::runBell(q); break; case MOD_SOUND: audio::tone(440,250); break;
    case MOD_DSP: ++s.dspTicks; (void)dsp::metrics(); break; case MOD_LAB: ++s.labTicks; lab::tick(); break;
    case MOD_AI: ++s.aiTicks; ai::generate(); break; case MOD_NETWORK: network::tick(); gate::tick(); break;
    case MOD_PROJECTS: engine::saveProject(); break; case MOD_RF: ++s.rfSamples; break;
    case MOD_MARAUDER: securitylab::sample(); securitylab::analyze(); if(securitylab::report().mode==securitylab::LAB_SIMULATION) securitylab::runLabSimulation(); ++s.marauderFrames; break;
    case MOD_STUDIO: studio::trigger(60+(s.studioTicks&7),100); ++s.studioTicks; break; case MOD_SYSTEM: mission::refresh(); diag::tick(s.frame); recovery::heartbeat(); break;
    case MOD_ANIMAL: animal::analyze(); animal::synthesize(); ++s.animalTicks; break; case MOD_CODEX: codex::tick(); ++s.codexTicks; break;
    case MOD_HARMONIC: { harmonic::tick(); u32 hz=harmonic::outputMilliHz()/1000; if(hz<20)hz=20; if(hz>20000)hz=20000; audio::tone((u16)hz,150); ++s.harmonicTicks; break; }
    case MOD_SETTINGS: settings::adjust(1); break; default: ++s.coreTicks; break; }
    s.projectSaved=engine::projectExists();
}
void update(SystemState&s){
    scanKeys(); ++s.frame; if(s.frame==30) startDeferredServices(s);
    const u16 down=keysDown();
    if(down&KEY_START){ s.safeMode=!s.safeMode; s.screen=0; }
    touchPosition t; touchRead(&t); const bool touchDownNow=(keysHeld()&KEY_TOUCH)!=0;
    if(touchDownNow && !touchWasDown){ touchStartX=t.px; touchStartY=t.py; touchGestureConsumed=false; }
    if(touchDownNow && !touchGestureConsumed){
        const int dx=(touchStartX>=0)?(int)t.px-touchStartX:0, dy=(touchStartY>=0)?(int)t.py-touchStartY:0;
        if(dx>28||dx<-28||dy>28||dy<-28){
            if(s.screen==0){ if(dx>28)s.selectedModule=(s.selectedModule+1)%MOD_COUNT; else if(dx<-28)s.selectedModule=(s.selectedModule+MOD_COUNT-1)%MOD_COUNT; else if(dy>28)s.selectedModule=(s.selectedModule+4)%MOD_COUNT; else s.selectedModule=(s.selectedModule+MOD_COUNT-4)%MOD_COUNT; }
            else { if(dx>28)s.selectedModule=(s.selectedModule+1)%MOD_COUNT; else if(dx<-28)s.selectedModule=(s.selectedModule+MOD_COUNT-1)%MOD_COUNT; else s.screen=0; }
            touchGestureConsumed=true;
        }
    }
    if(!touchDownNow && touchWasDown && !touchGestureConsumed){
        if(s.screen==0){ touchPosition tap=t; tap.px=(touchStartX>=0)?touchStartX:t.px; tap.py=(touchStartY>=0)?touchStartY:t.py; touchHome(s,tap); }
        else if(t.py>180) s.screen=0;
        else if(s.selectedModule==MOD_SETTINGS){ if(t.px<85)settings::previousSetting(); else if(t.px>170)settings::nextSetting(); else settings::adjust(1); }
        else doAction(s);
    }
    if(!touchDownNow){ touchGestureConsumed=false; touchStartX=-1; touchStartY=-1; }
    touchWasDown=touchDownNow;
    if(s.screen==0){
        if(down&KEY_LEFT)s.selectedModule=(s.selectedModule+MOD_COUNT-1)%MOD_COUNT; if(down&KEY_RIGHT)s.selectedModule=(s.selectedModule+1)%MOD_COUNT;
        if(down&KEY_UP)s.selectedModule=(s.selectedModule+MOD_COUNT-4)%MOD_COUNT; if(down&KEY_DOWN)s.selectedModule=(s.selectedModule+4)%MOD_COUNT;
        if(down&KEY_A)s.screen=s.selectedModule+1;
    }
    if(s.screen!=0){
        if(down&KEY_B)s.screen=0;
        if(s.selectedModule==MOD_SETTINGS){
            if(down&KEY_UP||down&KEY_LEFT)settings::previousSetting(); if(down&KEY_DOWN||down&KEY_RIGHT)settings::nextSetting(); if(down&KEY_A)settings::adjust(1); if(down&KEY_X)settings::save();
            if(down&KEY_Y){settings::profile().theme=settings::THEME_AUTO;settings::profile().layout=settings::LAYOUT_MYSPACE;settings::save();} if(down&KEY_SELECT)settings::save();
        } else {
            if(down&KEY_LEFT)s.selectedModule=(s.selectedModule+MOD_COUNT-1)%MOD_COUNT; if(down&KEY_RIGHT)s.selectedModule=(s.selectedModule+1)%MOD_COUNT; if(down&KEY_A)doAction(s);
            if(down&KEY_X&&s.selectedModule==MOD_QUANTUM)quantum::runGrover2(q); if(down&KEY_X&&s.selectedModule==MOD_DSP){++s.dspTicks;(void)dsp::metrics();} if(down&KEY_X&&s.selectedModule==MOD_RF)++s.rfSamples;
            if(down&KEY_X&&s.selectedModule==MOD_NETWORK){network::tick();gate::tick();} if(down&KEY_X&&s.selectedModule==MOD_AI)ai::generate(); if(down&KEY_X&&s.selectedModule==MOD_PROJECTS)engine::saveProject();
            if(down&KEY_Y&&s.selectedModule==MOD_QUANTUM)quantum::measure(q); if(down&KEY_Y&&s.selectedModule==MOD_MARAUDER)securitylab::acknowledge(); if(down&KEY_L&&s.selectedModule==MOD_QUANTUM)quantum::runDeutschJozsa(q); if(down&KEY_R&&s.selectedModule==MOD_QUANTUM)quantum::runQFT2(q);
            if(down&KEY_L&&s.selectedModule==MOD_MARAUDER)securitylab::setMode(securitylab::PASSIVE_RF); if(down&KEY_R&&s.selectedModule==MOD_MARAUDER)securitylab::setMode(securitylab::LAB_SIMULATION); if(down&KEY_X&&s.selectedModule==MOD_STUDIO){audio::tone(660,180);++s.studioTicks;} if(down&KEY_Y&&s.selectedModule==MOD_SOUND)audio::stop();
            if(down&KEY_Y&&s.selectedModule==MOD_SYSTEM)recovery::heartbeat(); if(down&KEY_X&&s.selectedModule==MOD_SYSTEM)osfabric::cycle(); if(down&KEY_Y&&s.selectedModule==MOD_CORE)recovery::heartbeat(); if(down&KEY_Y&&s.selectedModule==MOD_AI)ai::generate(); if(down&KEY_Y&&s.selectedModule==MOD_LAB)lab::tick();
            if(down&KEY_X&&s.selectedModule==MOD_ANIMAL){animal::setDirection(animal::ANIMAL_TO_HUMAN);animal::analyze();} if(down&KEY_Y&&s.selectedModule==MOD_ANIMAL){animal::setDirection(animal::HUMAN_TO_ANIMAL);animal::synthesize();}
            if(down&KEY_L&&s.selectedModule==MOD_ANIMAL)animal::setSpecies((animal::Species)((animal::report().species+animal::SPECIES_COUNT-1)%animal::SPECIES_COUNT)); if(down&KEY_R&&s.selectedModule==MOD_ANIMAL)animal::setSpecies((animal::Species)((animal::report().species+1)%animal::SPECIES_COUNT));
            if(down&KEY_X&&s.selectedModule==MOD_HARMONIC)harmonic::nextPrime(); if(down&KEY_Y&&s.selectedModule==MOD_HARMONIC)harmonic::setVoid((s16)(harmonic::node().voidVector+1)); if(down&KEY_L&&s.selectedModule==MOD_HARMONIC)harmonic::setDampener(harmonic::node().dampener>100?harmonic::node().dampener-100:0); if(down&KEY_R&&s.selectedModule==MOD_HARMONIC)harmonic::setAmplifier(harmonic::node().amplifier+100);
            if(down&KEY_X&&s.selectedModule==MOD_CODEX)codex::init(); if(down&KEY_Y&&s.selectedModule==MOD_DSP)(void)dsp::metrics(); if(down&KEY_Y&&s.selectedModule==MOD_PROJECTS)engine::resetProject(); if(down&KEY_SELECT){quantum::reset(q);audio::stop();}
        }
        if(!s.safeMode)quantum::tick(q); dsp::tick();lab::tick();ai::tick();studio::tick();hil::tick();engine::tick(); if((s.frame&63)==0){(void)dsp::metrics();ai::generate();} if(s.selectedModule==MOD_NETWORK&&(s.frame&127)==0)network::tick();
        gate::tick();session::tick();graph::tick();governor::tick();recovery::heartbeat();diag::tick(s.frame);securitylab::tick();mission::tick();capacity::tick();++s.securityTicks;++s.missionTicks;settings::tick();i18n::tick();animal::tick();codex::tick();harmonic::tick();heritage::tick();osfabric::tick();
    }
    ui::update(s);
}
void render(const SystemState&s){ui::render(s);} void shutdown(){audio::stop();consoleClear();} quantum::Simulator& simulator(){return q;}
}
