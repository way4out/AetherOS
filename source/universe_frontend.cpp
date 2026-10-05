#include <nds.h>
#include <nds/arm9/camera.h>
#include <nds/ndma.h>
#include <stdio.h>
#include <stdlib.h>
#include "hardware/hardware_profile.h"
#include "hardware/dsi_capability_scan.h"
#include "storage/resource_store.h"
#include "storage/module_registry.h"
#include "core/universe_boot.h"
#include "core/capacity_engine.h"
#include "core/diagnostics.h"
#include "quantum/quantum_core.h"
#include "quantum/quantum_scan.h"
#include "quantum/quantum_visuals.h"
#include "audio/aether_audio.h"

extern "C" int legacy_shell_main(void);

static const char *roles[]={
  "QUANTUM CORE","DSi HARDWARE SCAN","CREATOR / LAB",
  "COMMS / GATEWAYS","RECOVERY / DIAGNOSTICS","STORAGE / RESOURCE VAULT"
};
static const int ROLE_COUNT=6;

static void draw_top(int cursor, bool help){
  consoleClear();
  printf("\x1b[36;1mUNIVERSE SIMULATOR+\x1b[37;1m\n");
  printf("\x1b[35;1mAETHEROS DSi QUANTUM KIT\x1b[37;1m\n");
  printf("================================\n");
  printf("REAL-TIME LOCAL SIMULATOR | DSi\n");
  printf("ROLE: %-22s\n\n", roles[cursor]);
  if(help){
    printf("UP/DOWN select  A launch\n");
    printf("B safe runtime  X diagnostics\n");
    printf("Y help  TOUCH select/launch\n");
    printf("Quantum/hardware scans are local.\n");
  }else{
    printf("A  LAUNCH SELECTED ROLE\n");
    printf("X  DIAGNOSTICS / STATUS\n");
    printf("Y  HELP\n");
    printf("B  DEFAULT SYSTEMS\n");
  }
}

static void draw_bottom(int cursor){
  consoleClear();
  printf("\x1b[36;1mUNIVERSE ROLE MATRIX\x1b[37;1m\n\n");
  for(int i=0;i<ROLE_COUNT;i++)
    printf("%c %d  %-24s\n", i==cursor?'>':' ', i+1, roles[i]);
  printf("\n--------------------------------\n");
  printf("TOUCH ROW = SELECT + LAUNCH\n");
  printf("A = OPEN | B = SAFE RETURN\n");
  printf("Unsupported hardware stays gated.\n");
}

static void vault_screen(PrintConsole &top, PrintConsole &bottom){
  consoleSelect(&top); consoleClear();
  printf("\x1b[36;1mRESOURCE VAULT\x1b[37;1m\n\n");
  printf("SD MEDIA PROFILE 4 GB\n");
  printf("APP CAPACITY     RUNTIME-DEPENDENT\n");
  printf("CACHE           %lu MiB\n",(unsigned long)aether::storage::cacheMiB());
  printf("CACHE STATUS     %s\n",aether::storage::vaultReady()?"READY":"UNAVAILABLE");
  printf("MODULES          %d\n",aether::storage::moduleCount());
  printf("\nA/B = RETURN");
  consoleSelect(&bottom); consoleClear();
  printf("MODULE REGISTRY\n\n");
  for(int i=0;i<aether::storage::moduleCount() && i<14;++i)
    printf("%02d %-18s\n",i+1,aether::storage::moduleName(i));
  printf("\nFull registry: REVF/STORAGE/\nMANIFEST/MODULES.TXT\n");
  while(1){ swiWaitForVBlank(); scanKeys(); if(keysDown()&(KEY_A|KEY_B)) return; }
}

static void hardware_scan_screen(PrintConsole &top, PrintConsole &bottom){
  auto s=aether::hardware::scan();
  aether::audio::tone(880,60);
  consoleSelect(&top); consoleClear();
  printf("\x1b[36;1mDSi CAPABILITY SCAN\x1b[37;1m\n\n");
  printf("DSi/TWL mode     %s\n",s.dsi?"YES":"NO");
  printf("Dual screen      %s\n",s.dualScreen?"YES":"NO");
  printf("Touch            %s\n",s.touch?"YES":"NO");
  printf("Buttons/D-pad    %s\n",s.buttons?"YES":"NO");
  printf("Audio            %s\n",s.audio?"YES":"NO");
  printf("SD/libfat        %s\n",s.sd?"YES":"NO");
  printf("Microphone       %s\n",s.microphone?"YES":"NO");
  printf("Camera interface %s\n",s.camera?"YES":"NO");
  printf("Scan score       %u/100\n",s.score);
  consoleSelect(&bottom); consoleClear();
  printf("HAPTICS / FEEDBACK\n\n");
  printf("Native rumble    NOT PRESENT\n");
  printf("Touch feedback   READY\n");
  printf("Audible feedback READY\n");
  printf("External modules HARDWARE-GATED\n\n");
  printf("This scan probes exposed DSi\n");
  printf("interfaces without pretending\n");
  printf("unsupported hardware exists.\n\n");
  printf("A/B = RETURN");
  while(1){ swiWaitForVBlank(); scanKeys(); if(keysDown()&(KEY_A|KEY_B)) return; }
}

static void quantum_screen(PrintConsole &top, PrintConsole &bottom){
  aether::quantum::Simulator q{};
  aether::quantum::init(q);
  auto t=aether::quantum::selfTest(q);
  int mode=0; unsigned quantumRescanPassed=0; unsigned quantumRescanTotal=0;
  while(1){
    if(mode==0){ aether::quantum::runBell(q); }
    else if(mode==1){ aether::quantum::runGrover2(q); }
    else if(mode==2){ aether::quantum::runDeutschJozsa(q); }
    else if(mode==3){ aether::quantum::runQFT2(q); }
    else { aether::quantum::runTeleportation(q); }
    aether::quantum::visuals::draw(top,bottom,q,quantumRescanPassed,quantumRescanTotal,(unsigned)q.shots);
    consoleSelect(&top);
    printf("\n\x1b[36;1mQUANTUM REAL-TIME SIMULATOR\x1b[37;1m\n");
    printf("BACKEND: LOCAL STATE VECTOR\n");
    printf("QUBITS:  %d\n",q.qubits);
    printf("ALGORITHM: %d/5\n",mode+1);
    printf("TEST: %u/%u PASS\n",t.passed,t.total);
    printf("3X RESCAN: %u/%u\n",quantumRescanPassed,quantumRescanTotal);
    printf("SHOTS: %d LAST: %d\n\n",q.shots,q.lastMeasurement);
    for(int i=0;i<(1<<q.qubits) && i<8;i++)
      printf("|%d>  P=%0.3f\n",i,q.probability[i]);
    consoleSelect(&bottom); consoleClear();
    printf("QUANTUM TEST MATRIX\n\n");
    printf("Bell        %s\n",t.bell?"PASS":"FAIL");
    printf("Grover-2    %s\n",t.grover?"PASS":"FAIL");
    printf("Deutsch-Jozsa %s\n",t.deutschJozsa?"PASS":"FAIL");
    printf("QFT-2       %s\n",t.qft?"PASS":"FAIL");
    printf("Teleport    %s\n",t.teleport?"PASS":"FAIL");
    printf("Measurement %s\n",t.measurement?"PASS":"FAIL");
    printf("\nA = NEXT ALGORITHM\nX = RESCAN TESTS\nB = RETURN");
    swiWaitForVBlank(); aether::boot::tick((unsigned)q.shots+1); scanKeys();
    u32 d=keysDown();
    if(d&KEY_A){mode=(mode+1)%5; aether::audio::tone(1200,40);}
    if(d&KEY_X){ quantumRescanPassed=0; quantumRescanTotal=0; for(int pass=0;pass<3;++pass){ t=aether::quantum::selfTest(q); quantumRescanPassed+=t.passed; quantumRescanTotal+=t.total; } aether::audio::tone(1600,50); }
    if(d&KEY_B) return;
  }
}

static void io_screen(PrintConsole &top, PrintConsole &bottom){
  bool cam=aether::hardware::cameraAvailable();
  bool mic=aether::hardware::microphoneAvailable();
  int camMode=0;
  u16* frame=(u16*)malloc(256*192*2);
  if(!frame){ cam=false; }
  videoSetMode(MODE_5_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  if(cam){
    cameraSelect(camMode?CAMERA_OUTER:CAMERA_INNER);
    bgInit(3,BgType_Bmp16,BgSize_B16_256x256,0,0);
  }
  while(1){
    consoleSelect(&top); consoleClear();
    printf("\x1b[36;1mDSi AUDIO / CAMERA I-O\x1b[37;1m\n\n");
    printf("CAMERA DRIVER  %s\n",cam?"READY":"UNAVAILABLE");
    printf("ACTIVE CAMERA   %s\n",camMode?"OUTER":"INNER");
    printf("MICROPHONE      %s\n",mic?"READY":"UNAVAILABLE");
    printf("AUDIO OUT       READY\n");
    printf("TOUCH           READY\n");
    printf("\nA = SWITCH CAMERA\n");
    printf("X = AUDIO TEST\n");
    printf("Y = MIC TEST\n");
    printf("B = RETURN\n");
    consoleSelect(&bottom); consoleClear();
    printf("LIVE I/O CHANNELS\n\n");
    printf("Speaker/PCM     ACTIVE\n");
    printf("Mic input       %s\n",mic?"AVAILABLE":"GATED");
    printf("Camera preview  %s\n",cam?"AVAILABLE":"GATED");
    printf("Touch pixels    256 x 192\n");
    printf("Capture         DSi 640 x 480\n");
    printf("\nHardware is probed before use.\n");
    if(cam && frame && !cameraTransferActive()){
      if(cameraStartTransfer(frame,MCUREG_APT_SEQ_CMD_PREVIEW,0)){
        while(cameraTransferActive() || ndmaBusy(0)) swiWaitForVBlank();
        memcpy(bgGetGfxPtr(3),frame,256*192*2);
      }
    }
    swiWaitForVBlank(); scanKeys(); u32 d=keysDown();
    if(d&KEY_A && cam){ camMode^=1; cameraSelect(camMode?CAMERA_OUTER:CAMERA_INNER); }
    if(d&KEY_X){ aether::audio::tone(1000,180); }
    if(d&KEY_Y && mic){ aether::hardware::microphoneStart(); swiWaitForVBlank(); aether::hardware::microphoneStop(); }
    if(d&KEY_B) break;
  }
  if(cam) aether::hardware::cameraShutdown();
  if(frame) free(frame);
  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);
}

static void diagnostics(PrintConsole &top, PrintConsole &bottom){
  consoleSelect(&top); consoleClear();
  printf("\x1b[36;1mAETHEROS LIVE DIAGNOSTICS\x1b[37;1m\n\n");
  bool storageProfile=aether::hardware::ensureStorageProfile();
  printf("Frontend        ONLINE\n");
  printf("Touch/buttons   READY\n");
  printf("Dual screens    READY\n");
  printf("Boot services   %s\n",aether::boot::ready()?"READY":"CHECK");
  printf("Storage profile %s\n",storageProfile?"READY":"RUNTIME");
  printf("SD MEDIA        4 GB PROFILE\n");
  printf("Capacity        RUNTIME-DEPENDENT\n");
  auto cp=aether::capacity::report();
  auto dr=aether::diag::report();
  printf("WORKSPACE       %lu MB CLASS\n",(unsigned long)cp.workspaceMB);
  printf("INDEXED ASSETS   %lu\n",(unsigned long)cp.indexedAssets);
  printf("DIAG SCORE       %u\n",dr.score);
  consoleSelect(&bottom); consoleClear();
  printf("LIVE HEALTH\n\n");
  printf("Frame: %u\n",dr.frame);
  printf("Graph ticks: %u\n",dr.graphTicks);
  printf("Gateway count: %u\n",dr.gatewayOnline);
  printf("Faults: %u\n",dr.faults);
  printf("\nA/B = RETURN");
  while(1){swiWaitForVBlank();scanKeys();if(keysDown()&(KEY_A|KEY_B))return;}
}

extern "C" int universe_frontend(void){
  powerOn(POWER_ALL_2D);
  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);
  vramDefault();
  PrintConsole top,bottom;
  consoleInit(&top,0,BgType_Text4bpp,BgSize_T_256x256,22,3,true,true);
  consoleInit(&bottom,0,BgType_Text4bpp,BgSize_T_256x256,30,0,false,true);
  soundEnable();
  int cursor=0; bool help=false; unsigned frame=0;
  while(1){
    consoleSelect(&top); draw_top(cursor,help);
    consoleSelect(&bottom); draw_bottom(cursor);
    swiWaitForVBlank(); aether::boot::tick(++frame); scanKeys(); u32 d=keysDown();
    if(d&KEY_UP) cursor=(cursor+ROLE_COUNT-1)%ROLE_COUNT;
    if(d&KEY_DOWN) cursor=(cursor+1)%ROLE_COUNT;
    if(d&KEY_Y) help=!help;
    if(d&KEY_X){diagnostics(top,bottom);continue;}
    if(d&KEY_B) break;
    if(d&KEY_A){
      if(cursor==0){quantum_screen(top,bottom);continue;}
      if(cursor==1){hardware_scan_screen(top,bottom);continue;}
      if(cursor==3){io_screen(top,bottom);continue;}
      if(cursor==5){vault_screen(top,bottom);continue;}
      break;
    }
    if(d&KEY_TOUCH){
      touchPosition t; touchRead(&t);
      if(t.py>=32 && t.py<224){
        int r=((int)t.py-32)/32;
        if(r>=0&&r<ROLE_COUNT){cursor=r;aether::audio::tone(660,35);}
      }
    }
  }
  (void)aether::hardware::ensureStorageProfile();
  return legacy_shell_main();
}
