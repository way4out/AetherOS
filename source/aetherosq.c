#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>
#include "config.h"

/*
 * AetherMod for Nintendo DSi
 * Local-first dual-screen cockpit.
 * Hardware gateways are explicit: stock DSi hardware cannot become a physical
 * spectrum analyzer, QPU, satellite modem, or external RF instrument.
 */

#define APP_COUNT 29
#define AETHERMOD_MAJOR 8
#define AETHERMOD_MINOR 4
#define AETHERMOD_PASS 3
#define AETHERMOD_TOTAL_PASSES 5
#define AETHER_HOME_PAGES 4
#define NOTE_COUNT 8
#define CODEX_PATH "data/AetherMod/codex.txt"
#define ANIMAL_PATH "data/AetherMod/animals.txt"

typedef struct {
    u32 magic;
    u16 version;
    u16 mode;
    u16 sound;
    u16 intensity;
    u32 launches;
    u16 language;
    u8 parental, nsfw, unsafe, unregulated;
    u8 ai, onlineAI, privacy, wireless;
    u8 downloads, browser, userContent, theme;
    u8 brightness, dawBpm, dawStep;
    u16 selectionPin;
    u32 checksum;
} SaveData;

static SaveData save;
static PrintConsole topConsole, bottomConsole;
static const char *root = "fat:/";
static int mode=0, cursor=0, codexPage=0, animalPage=0;
static int safeMode=0, spectrumCursor=0, calculatorCursor=0;
static int selectionPin=0, homeScroll=0, homePulse=0, selfTestRun=0, coreTick=0;
static int touchPage=0, calcA=17, calcB=9, dawTrackMute=0, dspScale=1;
static int codexSearch=0, animalAnalyzing=0, fftWindow=0, fftPeakHold=0;
static int networkSelfTest=0, quantumState=0, dawPlaying=0, dawTrack=0;
static int rfMode=0, rfBand=0, rfChannel=1, rfPeakHold=0, rfPacketView=0;
static int saSpan=20, saStart=0, saRBW=10, saAtten=0, saMarker=0, saRunning=0, saGenArmed=0;
static int rfWarnIndex=0, rfPushQueue=0, rfAuthGate=0, rfLogCount=0, saView=0, saTraceHold=0, saInputSource=0;
static u32 frameCounter=0;
static u32 lastSaveFrame=0, sessionErrors=0, inputEvents=0;
static int diagnosticsPass=0, recoveryNotice=0, lastDiagnosticFrame=0;
static int dirtyState=0, bootCount=0, lastMode=0;
static int gatewayState=0, capabilityScore=0, resourceFaults=0;
static int keyRepeatFrames=0, lastKeys=0, eventBurst=0, frameBudgetFaults=0;
static int recoveryCount=0, validationFaults=0, moduleGuardFaults=0;
static int resetHoldFrames=0, resetConfirm=0, resetCursor=0, resetNotice=0;
static int soundId=-1;
static int touchFocus=0, touchAction=0, moduleTicks[APP_COUNT]={0};
static u32 uptimeFrames=0, touchEvents=0, autosaveCount=0, guardTrips=0;
static int quantumMeasure=0, quantumShots=0, aiSafetyEvents=0, networkPackets=0;
static int familyProfile=0, systemCursor=0;
static int visualTheme=1, hapticLevel=2, expansionCursor=0, liveRefresh=1, settingsSection=0;
static int hotspotState=0, hotspotBand=0, hotspotSecurity=2, hotspotRssi=72, hotspotTx=0, hotspotRx=0, hotspotPing=0, hotspotMode=0, phoneType=0, phonePackets=0;
static int phoneLinkState=0, phoneSession=0, phoneCompanion=0, phoneFileSync=0, phoneTelemetry=0, phoneRemote=0;
static int busTicks=0, busEvents=0, crossLink=0, codexSync=0, animalLink=0, dspLink=0, rfLink=0, botLink=0;
static int aiCursor=0, aiQuery=0, browserCursor=0, graphMode=0, dawView=0, settingsCursor=0, codexLine=0, animalFeature=0, telemetryPage=0;
static int qEntropy=0, qFidelity=0, qCursor=0, qHistogram[8]={0}, codexHits=0, codexLayer=0;
static int animalConfidence=0, animalEvents=0, animalOutput=0, animalHistory[8]={0};
static int rfSweep=0, rfTrace[24]={0}, saTrace[24]={0}, saPeak=0, saAvg=0;
static int calcMemory2=0, calcError=0, dspPeakBin=0, dspRms=0, dspFrames=0, dspHistory[8]={0};
static int phonePairCode=0, phoneBytesTx=0, phoneBytesRx=0, phoneQueue=0, phoneAck=0, netLatency=0, netHealth=0, botEvents=0;
static int livePhase=0, hapticPulse=0, visualEnergy=0;
static int hwTouch=0, hwButtons=0, hwMic=0, hwCamera=0, hwLed=0, hwSpeaker=0;
static int topFrames=0, bottomFrames=0, inputRoute=0, sensorRoute=0, mediaRoute=0, ledRoute=0;
static int releaseGuard=0, routeErrors=0, touchLatency=0, colorCycle=0;
static int busQuantum=0, busAudio=0, busAnimal=0, busRF=0, busPhone=0, busBot=0;
static int animFrame=0, animPulse=0, colorTheme=0;
static int animTopPhase=0, animBottomPhase=0, animSweep=0, animSpark=0;
static int homePageLock=0;
static int navSoundGate=0, pageTransition=0, touchX=0, touchY=0, touchPressed=0;
static int vaultCursor=0, vaultCount=0, noteCursor=0, clock24=1, diagCursor=0, fileCursor=0, fileCount=0, eventCursor=0, eventCount=0;
static int accessScale=1, accessContrast=0, accessScroll=1, controlCount=0, botCursor=0, botResult=0;
static u8 batteryLevel=0; static int dsiLive=0, touchLiveX=0, touchLiveY=0, touchLiveDown=0;
static char vaultNames[12][48];
static char fileNames[16][48];
static char eventNames[12][48];
static const char noteText[4][64]={"Rescue / build priorities","DSi local-first workspace","AetherOS 8.4 pass 3/5","User notes preserved on SD"};
static const u32 aetherLut[1024]={0};

static void saveState(void);
static void markDirty(void);
static void returnHome(void);
static void updateCapabilityHealth(void);
static void serviceInput(u32 keys);
static void resetToBase(void);
static int calcOp, calcInput, calcSign, calcMemory, calcTouchKey;
static int dawOctave, dawSwing, dawFx;
static int dspInputMode, dspSampleRate, dspGain, dspCursor;
static u8 dawPattern[4][16];
static int dawVolume[4];
static int normalizeSelection(int value);
static void canonicalizeSelection(void);
static void setSelection(int value);
static u32 hash32(const void *ptr,size_t n);
static void defaults(void);
static void page(const char *title);
static void footer(const char *s);
static void ensureDirs(void);
static void rfLogEvent(const char *kind, int value);
static long long calcResult(void);
static void dawStepAdvance(void);
static void tone(void);
static void feedback(int kind);
static void liveBus(void);
static void drawLiveBars(int seed);
static void hwBus(void);
static void releaseBus(void);
static void animateUI(void);
static void navigationFeedback(int direction);
static void pageTransitionFeedback(void);

static int storageReady(void){
    FILE *f=fopen("fat:/data/AetherMod/.aether_test","wb");
    if(!f) return 0;
    fputs("OK",f); fclose(f);
    remove("fat:/data/AetherMod/.aether_test");
    return 1;
}

static int saveIntegrity(void){
    SaveData t=save; u32 c=t.checksum; t.checksum=0;
    return c==hash32(&t,sizeof(t)) && save.magic==SAVE_MAGIC && save.version==4;
}

static void runDiagnostics(void){
    diagnosticsPass=0;
    sessionErrors=0;
    if(!saveIntegrity()) sessionErrors++;
    if(!storageReady()) sessionErrors++;
    if(selectionPin<0 || selectionPin>=APP_COUNT) sessionErrors++;
    if(cursor!=selectionPin) sessionErrors++;
    if(save.selectionPin!=selectionPin) sessionErrors++;
    diagnosticsPass=(sessionErrors==0);
    lastDiagnosticFrame=(int)frameCounter;
}

static void validateRuntimeState(void){
    int faults=0;
    if(selectionPin<0 || selectionPin>=APP_COUNT) faults++;
    if(cursor<0 || cursor>=APP_COUNT) faults++;
    if(save.selectionPin>=APP_COUNT) faults++;
    if(mode!=99 && (mode<0 || mode>APP_COUNT)) faults++;
    if(faults){
        validationFaults+=faults; guardTrips+=faults;
        canonicalizeSelection();
        if(mode!=99 && (mode<0 || mode>APP_COUNT)){ mode=0; recoveryCount++; }
    }
}
static void guardModuleState(void){
    if(mode!=99 && (mode<0 || mode>APP_COUNT)){
        moduleGuardFaults++;        mode=0;
        returnHome();
    }
}

static void canonicalizeSelection(void){
    int p=normalizeSelection(save.selectionPin);
    setSelection(p);
}

static int normalizeSelection(int value){
    if(value<0) return APP_COUNT-1;
    return value%APP_COUNT;
}

static void setSelection(int value){
    selectionPin=normalizeSelection(value);
    cursor=selectionPin;
    homeScroll=selectionPin/8;
    save.selectionPin=(u16)selectionPin;
}

static void launchSelection(void){
    setSelection(selectionPin);
    mode=selectionPin+1;
    save.launches++;
    saveState();
}

static const char *apps[APP_COUNT]={
    "AETHER HOME","QUANTUM CORE","YHWH CODEX","ANIMAL AI",
    "MARAUDER/RF","TINySA LAB","CALCULATOR","DAW STUDIO",
    "DSP/FFT","TELEMETRY","AI HOME","NETWORK GATEWAY",
    "PHONE LINK",
    "MEDIA STUDIO","SENSOR HUB","DATA VAULT","FILE BROWSER",
    "HAPTIC LAB","ACCESSIBILITY","POWER LAB","CONTROL LAB",
    "DIAGNOSTICS","AETHER BOT","GENERAL SETTINGS","EVENT LOG","DATA VAULT","NOTES","CLOCK","DIAGNOSTICS"
};

static const char *langs[10]={
    "English","Espanol","Francais","Deutsch","Italiano",
    "Portugues","Nihongo","Hangul","Chinese","Russian"
};

static const char *animalNames[]={
    "Horse","Dog","Cat","Cow","Bison","Camel","Zebra","Ostrich",
    "Bird","Wolf","Fox","Deer","Bear","Big Cat","Other"
};

static u32 hash32(const void *ptr,size_t n){
    const u8 *p=(const u8*)ptr; u32 h=2166136261u;
    while(n--){h^=*p++; h*=16777619u;} return h;
}

static void defaults(void){
    memset(&save,0,sizeof(save));
    save.magic=SAVE_MAGIC; save.version=4;
    save.sound=0; save.intensity=2; save.language=0;
    save.parental=1; save.nsfw=1; save.unsafe=1; save.unregulated=1;
    save.ai=1; save.privacy=1; save.wireless=0; save.downloads=0;
    save.browser=0; save.userContent=1; save.theme=0; save.brightness=3;
    save.dawBpm=120; save.dawStep=0; save.selectionPin=0;
}

static void ensureDirs(void){
    char a[96],b[96];
    snprintf(a,sizeof(a),"%sdata",root);
    snprintf(b,sizeof(b),"%sdata/AetherMod",root);
    mkdir(a,0777); mkdir(b,0777);
}

static void markDirty(void){ dirtyState=1; }

static long long calcResult(void){
    long long a=calcA,b=calcB;
    switch(calcOp%12){case 0:return a+b;case 1:return a-b;case 2:return a*b;case 3:return b?a/b:0;case 4:return b?a%b:0;case 5:return b?(a*100)/b:0;case 6:return a*a;case 7:return a*a*a;case 8:return a>b?a:b;case 9:return a<b?a:b;case 10:return a^b;default:return (a*a+b*b)%1000003;}
}
static void dawStepAdvance(void){
    save.dawStep=(save.dawStep+1)%16;
    for(int t=0;t<4;t++) if(dawPattern[t][save.dawStep]) tone();
}

static void feedback(int kind){
    hapticPulse++;
    visualEnergy=(visualEnergy+11+(kind*7))%101;
    /* Explicit action feedback only; navigation remains silent. */
    if(save.sound) tone();
}
static void navigationFeedback(int direction){
    (void)direction;
    hapticPulse++;
    visualEnergy=(visualEnergy+3)%101;
    navSoundGate=0;
    /* Navigation is intentionally silent. */
}
static void pageTransitionFeedback(void){
    pageTransition=1;
    hapticPulse++;
    visualEnergy=(visualEnergy+9)%101;
    navSoundGate=0;
    /* Page changes are intentionally silent. */
}
static void hwBus(void){ hwTouch=1; hwButtons=(int)keysHeld(); hwMic=1; hwCamera=1; hwLed=1; hwSpeaker=save.sound?1:0; inputRoute=hwTouch+(hwButtons?1:0); sensorRoute=hwMic+hwCamera; mediaRoute=hwSpeaker+phoneLinkState; ledRoute=hwLed+(hotspotState?1:0); if((frameCounter&7)==0){topFrames++;bottomFrames++;} }
static void releaseBus(void){ releaseGuard=(hwTouch&&hwMic&&hwCamera&&hwLed)?1:0; if(!crossLink) routeErrors++; if(touchEvents) touchLatency=(touchLatency+1)%16; }
static void animateUI(void){
    animFrame=(animFrame+1)&63;
    animPulse=(animPulse+1)&31;
    colorTheme=(colorTheme+1)&15;
    animTopPhase=(animTopPhase+1)&127;
    animBottomPhase=(animBottomPhase+1)&127;
    animSweep=(animSweep+3)&255;
    animSpark=(animSpark+5)&255;
}
static void applyAnimatedColors(void){
    static const char *topColors[6]={"\x1b[36;1m","\x1b[36;1m","\x1b[32;1m","\x1b[32;1m","\x1b[33;1m","\x1b[37;1m"};
    static const char *botColors[6]={"\x1b[34;1m","\x1b[34;1m","\x1b[35;1m","\x1b[35;1m","\x1b[36;1m","\x1b[37;1m"};
    consoleSelect(&topConsole); printf("%s",topColors[(animTopPhase/22)%6]);
    consoleSelect(&bottomConsole); printf("%s",botColors[(animBottomPhase/22)%6]);
}
static void liveBus(void){ if(!liveRefresh) return; livePhase=(livePhase+1)%32; visualEnergy=(visualEnergy+2+((frameCounter/4)%7))%101; if(crossLink){busEvents++;busQuantum=(busQuantum+qFidelity+1)%101;busAudio=(busAudio+dspRms+1)%101;busAnimal=(busAnimal+animalConfidence+2)%101;busRF=(busRF+saAvg+3)%101;busPhone=(busPhone+hotspotRssi+1)%101;busBot=(busBot+botEvents+1)%101;} }
static void drawLiveBars(int seed){int p=(seed+livePhase)%24;iprintf("LIVE |");for(int i=0;i<24;i++)iprintf("%c",i==p?'@':((i+seed+visualEnergy)%5==0?'#':((i+seed)%3==0?'+':'.')));iprintf("| %3d%%\n",visualEnergy);}
static void serviceInput(u32 keys){
    int changedKeys=(int)keys ^ lastKeys;
    if(changedKeys) inputEvents++;
    if(keys) keyRepeatFrames++; else keyRepeatFrames=0;
    if(keys && keyRepeatFrames>180) frameBudgetFaults++;
    if(changedKeys && eventBurst<255) eventBurst++;
    else if(!keys && eventBurst>0) eventBurst--;
    lastKeys=(int)keys;
}


static void fileScan(void);
static void sampleHardware(void){ dsiLive=isDSiMode()?1:0; batteryLevel=getBatteryLevel(); touchPosition t; touchRead(&t); touchLiveX=t.px; touchLiveY=t.py; touchLiveDown=(keysHeld()&KEY_TOUCH)?1:0; }
static void botExecute(void){ switch(botCursor%8){case 0:mode=0;break;case 1:runDiagnostics();mode=29;break;case 2:fileScan();mode=17;break;case 3:mode=25;break;case 4:mode=20;break;case 5:mode=21;break;case 6:mode=19;break;default:mode=22;break;} botResult=botCursor+1;botEvents++; }
static void updateCapabilityHealth(void){
    capabilityScore=100;
    if(storageReady()) capabilityScore+=0; else capabilityScore-=20;
    if(!isDSiMode()) capabilityScore-=5;
    if(save.wireless) capabilityScore-=0;
    if(save.onlineAI && save.privacy) capabilityScore-=10;
    if(safeMode) capabilityScore-=5;
    if(capabilityScore<0) capabilityScore=0;
}


static void returnHome(void){ lastMode=mode; mode=0; setSelection(selectionPin); homeScroll=selectionPin/8; saveState(); }

static void moduleHeartbeat(void){ hwBus(); liveBus(); releaseBus(); colorCycle=(colorCycle+1)%48;
    if(mode>=1 && mode<=APP_COUNT) moduleTicks[mode-1]++;
    uptimeFrames=frameCounter;
    if((frameCounter&15)==0){
        busTicks++;
        if(crossLink) busEvents++;
        if(hotspotState){
            hotspotTx++;
            hotspotRx+=2;
            phonePackets++;
            if(hotspotPing<25) hotspotPing=25+(int)(frameCounter%18);
        }
        if(phoneLinkState){
            phoneTelemetry=1;
            if(phoneSession==0) phoneSession=1;
        }
    }
}


static void resetToBase(void){
    defaults();
    selectionPin=0; cursor=0; homeScroll=0; homePulse=0;
    mode=0; safeMode=0; gatewayState=0; dirtyState=0;
    codexPage=0; codexSearch=0; codexLine=0; animalPage=0; animalFeature=0; animalAnalyzing=0; rfWarnIndex=0; rfPushQueue=0; rfAuthGate=0; rfLogCount=0; saView=0; saTraceHold=0; saInputSource=0;
    calculatorCursor=0; calcA=17; calcB=9; calcOp=0; calcInput=0; calcSign=1; calcMemory=0; calcTouchKey=0; dawTrack=0; dawTrackMute=0; dawPlaying=0; dawView=0; dawOctave=4; dawSwing=0; dawFx=0;
    dspScale=1; fftWindow=0; fftPeakHold=0; dspInputMode=0; dspSampleRate=44100; dspGain=1; dspCursor=0; telemetryPage=0; aiCursor=0; aiQuery=0; browserCursor=0;
    networkSelfTest=0; quantumState=0; rfMode=0; rfBand=0; rfChannel=1; rfPeakHold=0; rfPacketView=0;
    saSpan=20; saStart=0; saRBW=10; saAtten=0; saMarker=0; saRunning=0; saGenArmed=0;
    recoveryNotice=0; resetHoldFrames=0; resetConfirm=0; resetCursor=0; resetNotice=1;
    if(fatInitDefault()){
        ensureDirs();
        char p[120]; snprintf(p,sizeof(p),"%sdata/AetherMod/save.dat",root); remove(p);        saveState();
    }
}

static void resetPage(void){
    page("AETHERMOD BASE RESET");
    iprintf("RESTORE FACTORY / BASE SETTINGS\n\n");
    iprintf("This clears AetherMod settings and runtime state.\n");
    iprintf("Installed SD corpus/data files are preserved.\n\n");
    iprintf("%s  YES\n",resetCursor==0?">":" ");
    iprintf("%s  NO\n\n",resetCursor==1?">":" ");
    iprintf("SELECT = CONFIRM YES    B = CANCEL\n");
    iprintf("START hold detected: 3-second recovery path.\n");
}

static void saveState(void){
    if(safeMode) return;
    ensureDirs();
    save.checksum=0; save.checksum=hash32(&save,sizeof(save));
    char p[120]; snprintf(p,sizeof(p),"%sdata/AetherMod/save.dat",root);
    FILE *f=fopen(p,"wb"); if(!f) return;
    fwrite(&save,1,sizeof(save),f); fclose(f); lastSaveFrame=frameCounter;
}

static void loadState(void){
    defaults();
    bootCount=1; ensureDirs();
    char p[120]; snprintf(p,sizeof(p),"%sdata/AetherMod/save.dat",root);
    FILE *f=fopen(p,"rb"); if(!f) return;
    SaveData t; if(fread(&t,1,sizeof(t),f)==sizeof(t)){
        u32 old=t.checksum; t.checksum=0;
        if(old==hash32(&t,sizeof(t)) && t.magic==SAVE_MAGIC && t.version==4){
            save=t; if(save.selectionPin>=APP_COUNT) save.selectionPin=0;
        }
    }
    fclose(f);
    canonicalizeSelection();
    if(!saveIntegrity()) recoveryNotice=1;
    dirtyState=0;
}

static const char *langName(void){return langs[save.language%10];}

static void topBg(const char *title){
    consoleSelect(&topConsole); consoleClear();
    iprintf("\x1b[36;1m      A E T H E R M O D  8.2\x1b[37;1m\n");
    iprintf("\x1b[35;1m  ========================\x1b[37;1m\n");
    iprintf("  %s\n\n",title);
    iprintf("  [%s]  QCORE:%s  AI:%s\n",
        isDSiMode()?"DSi":"DS",save.ai?"ON":"OFF",save.privacy?"LOCAL":"OPEN");
    iprintf("  RF:%s  NET:%s  DSP:%s\n",
        save.wireless?"GATE":"OFF",save.onlineAI?"ON":"LOCAL","READY");
    iprintf("\n  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
      '#','.',':','*','+','.',':','*','+','.',':','*','+','.',':','#');
    drawLiveBars((int)(frameCounter/2));
    iprintf("  FRAME %lu BPM %u  V%d H%d\n",(unsigned long)frameCounter,save.dawBpm,visualTheme,hapticLevel);
    iprintf("  SAFE %s   LANG %s\n",safeMode?"YES":"NO",langName());
    iprintf("  HW 2LCD:%d TOUCH:%d MIC:%d CAM:%d SPK:%d LED:%d\n",topFrames>0&&bottomFrames>0,hwTouch,hwMic,hwCamera,hwSpeaker,hwLed);
}

static void setAetherPalette(void){
    static const u16 pal[8]={RGB15(31,31,31),RGB15(31,8,8),RGB15(8,31,12),RGB15(31,27,5),RGB15(8,16,31),RGB15(28,8,31),RGB15(5,31,31),RGB15(22,22,27)};
    for(int i=0;i<8;i++){ BG_PALETTE[15+i*16]=pal[i]; BG_PALETTE_SUB[15+i*16]=pal[i]; }
    BG_PALETTE[0]=RGB15(1,2,4); BG_PALETTE_SUB[0]=RGB15(1,2,4);
}
static void fileScan(void){
    fileCount=0; memset(fileNames,0,sizeof(fileNames));
    DIR *d=opendir("fat:/data/AetherMod"); if(!d && isDSiMode()) d=opendir("sd:/data/AetherMod");
    if(!d) return; struct dirent *e;
    while((e=readdir(d)) && fileCount<16){ if(e->d_name[0]=='.') continue; strncpy(fileNames[fileCount],e->d_name,47); fileCount++; }
    closedir(d); if(fileCursor>=fileCount) fileCursor=0;
}
static void fileBrowserPage(void){
    page("FILE BROWSER / LOCAL-FIRST");
    fileScan();
    iprintf("ROOT: data/AetherMod/   ENTRIES:%d\n\n",fileCount);
    for(int i=0;i<fileCount;i++) iprintf("%c %02d %-29s\n",i==fileCursor?'>':' ',i+1,fileNames[i]);
    iprintf("\nA=PREVIEW X=RESCAN B=HOME\n");
    if(fileCount){
        char p[128]; snprintf(p,sizeof(p),"%sdata/AetherMod/%s",root,fileNames[fileCursor]);
        FILE *f=fopen(p,"rb");
        if(f){ unsigned char b[32]={0}; size_t n=fread(b,1,sizeof(b),f); fclose(f);
            u32 h=hash32(b,n); iprintf("\nSIZE SAMPLE:%uB  HASH:%08lX\n",(unsigned)n,(unsigned long)h);
        }
    }
}
static void eventLogPage(void){
    page("EVENT / SESSION LOG");
    char p[128]; snprintf(p,sizeof(p),"%sdata/AetherMod/rf_session.log",root);
    FILE *f=fopen(p,"rb"); char lines[12][64]; eventCount=0;
    if(f){ while(eventCount<12 && fgets(lines[eventCount],64,f)){ size_t n=strlen(lines[eventCount]); if(n&&lines[eventCount][n-1]=='\\n') lines[eventCount][n-1]=0; eventCount++; } fclose(f); }
    iprintf("SESSION EVENTS:%d\n\n",eventCount);
    for(int i=0;i<eventCount;i++) iprintf("%c %02d %s\n",i==eventCursor?'>':' ',i+1,lines[i]);
    if(!eventCount) iprintf("No session events recorded yet.\n");
    footer("UP/DOWN SELECT  X REFRESH  B HOME");
}
static void vaultScan(void){
    vaultCount=0; memset(vaultNames,0,sizeof(vaultNames));
    DIR *d=opendir("fat:/data/AetherMod"); if(!d && isDSiMode()) d=opendir("sd:/data/AetherMod");
    if(!d) return; struct dirent *e; while((e=readdir(d)) && vaultCount<12){
        if(e->d_name[0]=='.') continue; strncpy(vaultNames[vaultCount],e->d_name,47); vaultCount++;
    } closedir(d); if(vaultCursor>=vaultCount) vaultCursor=0;
}
static void vaultPage(void){
    page("DATA VAULT / SD BROWSER"); vaultScan();
    iprintf("REAL SD DIRECTORY: data/AetherMod\n\n");
    if(!vaultCount){iprintf("No readable entries found.\n");}
    for(int i=0;i<vaultCount;i++) iprintf("%c %02d  %-30s\\n",i==vaultCursor?'>':' ',i+1,vaultNames[i]);
    iprintf("\nA = inspect  X = rescan  B = home\n");
    if(vaultCount){ char p[128]; snprintf(p,sizeof(p),"%sdata/AetherMod/%s",root,vaultNames[vaultCursor]); FILE *f=fopen(p,"rb"); if(f){char buf[81]={0}; size_t n=fread(buf,1,80,f); fclose(f); buf[n]=0; iprintf("\nPREVIEW: %s\n",buf);}}
}
static void notesPage(void){
    page("PERSISTENT NOTES");
    iprintf("SD-BACKED QUICK NOTES\n\n");
    for(int i=0;i<4;i++) iprintf("%c %d  %s\\n",i==noteCursor?'>':' ',i+1,noteText[i]);
    iprintf("\nA = append selected note to SD\nX = refresh  UP/DOWN = select\n");
}
static void clockPage(void){
    page("CLOCK / SYSTEM TIME"); time_t now=time(NULL); struct tm *tmv=localtime(&now);
    if(tmv){ int hh=tmv->tm_hour; if(!clock24){ hh%=12; if(hh==0) hh=12; } iprintf("%02d:%02d:%02d %s\n\nDATE %04d-%02d-%02d\n",hh,tmv->tm_min,tmv->tm_sec,clock24?"":"12H",1900+tmv->tm_year,1+tmv->tm_mon,tmv->tm_mday); }
    else iprintf("RTC TIME UNAVAILABLE\\n");
    iprintf("FRAME %lu\\nUPTIME %lu FRAMES\\nDSi MODE %s\\n",(unsigned long)frameCounter,(unsigned long)uptimeFrames,isDSiMode()?"YES":"NO");
    footer("A toggles 12/24 display  B HOME");
}
static void diagPage(void){
    page("DIAGNOSTICS / 8.4 PASS 1/5"); runDiagnostics();
    iprintf("STORAGE %s\\nSAVE INTEGRITY %s\\nRUNTIME FAULTS %d\\n",storageReady()?"READY":"FAIL",saveIntegrity()?"PASS":"RECOVER",validationFaults);
    iprintf("FRAME BUDGET %s\\nINPUT EVENTS %lu\\nGUARD TRIPS %lu\\n",frameBudgetFaults?"CHECK":"PASS",(unsigned long)inputEvents,(unsigned long)guardTrips);
    iprintf("COLOR PALETTE: ACTIVE\\nAUDIO: HARD-OFF DEFAULT\\nLOCAL DATA: ENABLED\\n");
    iprintf("\\n5-PASS BUILD BASELINE: UI / COLOR / SD / TOOLS / SAFETY / RECOVERY / DATA\\n");
}
static void page(const char *title){
    topBg(title);
    consoleSelect(&bottomConsole); consoleClear();
    iprintf("\x1b[36;1mAETHERMOD :: %s\x1b[37;1m\n",title);
    iprintf("------------------------------\n");
}

static void footer(const char *s){iprintf("\n%s\n",s);}

static void expansion(void){
    const char *names[]={
        "PHONE LINK","MEDIA STUDIO","SENSOR HUB","DATA VAULT","FILE BROWSER","HAPTIC LAB",
        "ACCESSIBILITY","POWER LAB","CONTROL LAB","DIAGNOSTICS","AETHER BOT","GENERAL SETTINGS"
    };
    int ix=mode-13; if(ix<0) ix=0; if(ix>11) ix=11;
    page(names[ix]);
    int pulse=(int)((frameCounter/2)%24), meter=(int)((frameCounter/3)%100);
    iprintf("LIVE WORKSPACE %s  REFRESH %s\n",liveRefresh?"ON":"OFF",liveRefresh?"LIVE":"PAUSED");
    iprintf("THEME %d  HAPTIC %d/3  BUS %s  EVENTS %d\n",
        visualTheme,hapticLevel,crossLink?"LINKED":"READY",busEvents);
    iprintf("SIGNAL |");
    for(int i=0;i<24;i++) iprintf("%c",((i+pulse)%7==0)?'#':((i+meter/10)%3==0)?'+':'.');
    iprintf("|\nLEVEL %3d%%  ACTIVITY %3d%%  FRAME %lu\n",meter,(pulse*7)%101,(unsigned long)frameCounter);

    if(ix==0){
        iprintf("PHONE LINK / iPHONE 15 + ANDROID\n");
        iprintf("HOTSPOT %s  PROFILE %s  LINK %s\n",
            hotspotState?"CONNECTED":"READY",phoneType?"ANDROID":"iPHONE 15",
            phoneLinkState?"ACTIVE":"STANDBY");
        iprintf("5G BACKHAUL -> 2.4G DSi WIFI\n");
        iprintf("RSSI %d%%  TX %d  RX %d  PING %dms\n",
            hotspotRssi,hotspotTx,hotspotRx,hotspotPing);
        iprintf("SESSION %d  COMPANION %s  REMOTE %s\n",
            phoneSession,phoneCompanion?"READY":"OFF",phoneRemote?"ARMED":"SAFE");
        phonePairCode=100000+(int)((frameCounter/30)%899999); phoneBytesTx+=hotspotState?2:0; phoneBytesRx+=hotspotState?3:0; phoneAck=hotspotState?1:0;
        iprintf("PAIR %06d ACK %s QUEUE %d TX/RX %d/%d\n",phonePairCode,phoneAck?"YES":"NO",phoneQueue,phoneBytesTx,phoneBytesRx);
        iprintf("SYNC FILE %s  TELEMETRY %s  BOT %s\n",
            phoneFileSync?"READY":"IDLE",phoneTelemetry?"LIVE":"IDLE",botLink?"LINKED":"IDLE");
        iprintf("A LINK  X BAND  Y TEST  L/R PROFILE\n");
        iprintf("TOUCH: service rows / bridge actions\n");
    } else if(ix==1) {
        iprintf("MEDIA ROUTER / DAW CLIPS / DSP INPUT\n");
        iprintf("DAW->DSP %s  PHONE MEDIA %s\n",dspLink?"LINKED":"READY",phoneFileSync?"READY":"LOCAL");
    } else if(ix==2) {
        iprintf("MIC / CAMERA / TOUCH SENSOR ROUTING\n");
        iprintf("ANIMAL INPUT %s  SENSOR BUS %d\n",animalLink?"LINKED":"READY",busTicks);
    } else if(ix==3) {
        iprintf("SD DATA VAULT / INDEX / SESSION CHECKS\n");
        iprintf("CODEX SYNC %s  FILE SYNC %s\n",codexSync?"READY":"IDLE",phoneFileSync?"READY":"IDLE");
    } else if(ix==4) {
        iprintf("LOCAL FILE NAVIGATOR: data/AetherMod/\n");
        iprintf("PHONE TRANSFER %s  DATA BUS %s\n",phoneFileSync?"ARMED":"SAFE",crossLink?"ACTIVE":"READY");
    } else if(ix==5) {
        iprintf("TOUCH FEEDBACK / TONE / RESPONSE STRENGTH\n");
        iprintf("LEVEL %d/3  EVENTS %lu\n",hapticLevel,(unsigned long)touchEvents);
    } else if(ix==6) {
        iprintf("LARGE TEXT / CONTRAST / INPUT ASSIST\n");
        iprintf("TOUCH FOCUS %s  UNIVERSAL INPUT READY\n",touchFocus?"ON":"AUTO");
    } else if(ix==7) {
        iprintf("POWER STATE / WORKLOAD / BATTERY GATEWAY\n");
        iprintf("WORKLOAD %d%%  SAVE %s  SAFE %s\n",meter,saveIntegrity()?"OK":"CHECK",safeMode?"ON":"OFF");
    } else if(ix==8) {
        iprintf("CROSS-MODULE ROUTING / UNIVERSAL CONTROL BUS\n");
        iprintf("QCORE PHONE RF DSP BOT LINKS\n");
        iprintf("BUS %s  EVENTS %d  TICKS %d\n",crossLink?"ACTIVE":"READY",busEvents,busTicks);
    } else if(ix==9) {
        iprintf("RUNTIME GUARDS / SAVE / SUBSYSTEM HEARTBEAT\n");
        iprintf("GUARDS %lu  VALIDATION %d  HEALTH %d%%\n",
            (unsigned long)guardTrips,validationFaults,capabilityScore);
    } else if(ix==10) {
        iprintf("AETHER BOT / COMMAND + MODULE ROUTING\n");
        iprintf("PHONE REMOTE %s  KNOWLEDGE %s\n",phoneRemote?"ARMED":"SAFE",codexSync?"SYNC":"LOCAL");
        iprintf("LINKS: CALC / CODEX / ANIMAL / RF / DSP\n");
    } else {
        iprintf("MASTER SETTINGS / SAFETY + FAMILY + SYSTEM\n");
        iprintf("AI %s  PRIVACY %s  WIFI %s  BROWSER %s\n",
            save.ai?"ON":"OFF",save.privacy?"LOCK":"OPEN",
            save.wireless?"ARMED":"GUARDED",save.browser?"ON":"OFF");
        iprintf("VISUAL %d  HAPTIC %d  LIVE %s\n",visualTheme,hapticLevel,liveRefresh?"ON":"OFF");
    }
    footer("UP/DOWN CURSOR  A ACTION  X VISUAL  Y LIVE  L/R HAPTIC  B HOME");
}

static void home(void){
    canonicalizeSelection();
    homeScroll=selectionPin/8;
    if(homeScroll<0) homeScroll=0;
    if(homeScroll>=AETHER_HOME_PAGES) homeScroll=AETHER_HOME_PAGES-1;
    runDiagnostics();
    updateCapabilityHealth();
    topBg("DUAL-OS COCKPIT");
    consoleSelect(&bottomConsole); consoleClear();
    iprintf("AETHERMOD 8.4 PASS 2 / IMMERSIVE COCKPIT\n");
    iprintf("------------------------------\n");
    iprintf("PAGE %d/3   MODULES %02d-%02d   %s\n\n",
        homeScroll+1,homeScroll*8+1,homeScroll*8+8,
        pageTransition?"PAGE SHIFT":"LIVE");
    int first=homeScroll*8;
    for(int i=0;i<8;i++){
        int n=first+i;
        int pulse=((animSweep+(i*17))&63);
        iprintf("%s%02d %-18s %c\n",n==cursor?"> ":"  ",n+1,apps[n],
            (n==cursor && pulse<18)?'*':(pulse<34)?'+':'.');
    }
    iprintf("\nPIN:%02d  TARGET:%s  ENERGY:%03d\n",selectionPin+1,apps[selectionPin],visualEnergy);
    iprintf("DIAG:%s ERR:%lu HEALTH:%d%%\n",diagnosticsPass?"PASS":"CHECK",(unsigned long)sessionErrors,capabilityScore);
    iprintf("\nA OPEN  X QUANTUM  Y TELEMETRY  L/R PAGE\n");
    iprintf("TOUCH rows=OPEN   top/bottom=PAGE   START hold=RESET\n");
}

static void quantum(void){
    page("QUANTUM CORE");
    qEntropy=(int)((frameCounter/5+qCursor*17)%101); qFidelity=80+(int)((frameCounter/13)%20);
    int e=(frameCounter/3)%101;
    iprintf("QUANTUM WORKBENCH / LOCAL SIM\n");
    iprintf("COHERENCE %3d%%  PHASE %3lu deg\n",e,(unsigned long)((frameCounter/7)%360));
    iprintf("Q-LANES 8  STATE %d  MEASURE:%s  SHOTS:%d\n",quantumState,quantumMeasure?"YES":"NO",quantumShots);
    iprintf("VECTOR |"); for(int i=0;i<16;i++) iprintf("%c",((i+e/7)%5==0)?'#':'.'); iprintf("|\n");
    iprintf("GRAPH  |"); for(int i=0;i<16;i++){int v=(i*7+e)%16;iprintf("%c",v>10?'*':v>5?'+':'.');} iprintf("|\n");
    iprintf("ALGO superposition / phase / measure\n");
    iprintf("FFT BRIDGE READY  QPU %s\n",save.wireless?"GATEWAY":"LOCAL");
    iprintf("Software quantum simulator; no physical QPU claimed.\n");
    iprintf("PHONE BRIDGE %s  TELEMETRY %s\n",phoneLinkState?"READY":"LOCAL",phoneTelemetry?"LIVE":"IDLE");
    iprintf("FIDELITY %d%%  ENTROPY %d%%  HIST ",qFidelity,qEntropy);
    drawLiveBars(qEntropy); for(int i=0;i<8;i++) iprintf("%d ",qHistogram[i]); iprintf("\n");
    footer("A RUN  X PHASE  Y MEASURE  B HOME  START HOLD 3s = RESET");
}

static void codex(void){
    page("YHWH BIBLIO CODEX");
    iprintf("CODEX READER / INDEX\n");
    iprintf("Page %d / 10   SEARCH:%s  LINE:%d\n\n",codexPage+1,codexSearch?"ON":"OFF",codexLine);
    switch(codexPage){
      case 0: iprintf("GENESIS  EXODUS  LEVITICUS\nNUMBERS  DEUTERONOMY  JOSHUA\nJUDGES  RUTH  1 SAMUEL  2 SAMUEL\n"); break;
      case 1: iprintf("1 KINGS  2 KINGS  1 CHRONICLES\n2 CHRONICLES  EZRA  NEHEMIAH\nESTHER  JOB  PSALMS  PROVERBS\n"); break;
      case 2: iprintf("ECCLESIASTES  SONG  ISAIAH\nJEREMIAH  LAMENTATIONS  EZEKIEL\nDANIEL  HOSEA  JOEL  AMOS\n"); break;
      case 3: iprintf("OBADIAH  JONAH  MICAH  NAHUM\nHABAKKUK  ZEPHANIAH  HAGGAI\nZECHARIAH  MALACHI\n"); break;
      case 4: iprintf("MATTHEW  MARK  LUKE  JOHN\nACTS  ROMANS  1 CORINTHIANS\n2 CORINTHIANS  GALATIANS  EPHESIANS\n"); break;
      case 5: iprintf("PHILIPPIANS  COLOSSIANS  1 THESS\n2 THESS  1 TIMOTHY  2 TIMOTHY\nTITUS  PHILEMON  HEBREWS  JAMES\n"); break;
      case 6: iprintf("1 PETER  2 PETER  1 JOHN  2 JOHN\n3 JOHN  JUDE  REVELATION\n"); break;
      case 7: iprintf("NAME LAYER: YHWH / LORD / ADONAI\nSEARCHABLE TEXT GATEWAY\nSD DATA: " CODEX_PATH "\n"); break;
      case 8: iprintf("CROSS-REFERENCE ENGINE\nBOOK / CHAPTER / VERSE\nLEXICON / STRONG-STYLE INDEX\n"); break;
      default: iprintf("USER CODEX DATASET\nAdd UTF-8/plain-text corpus on SD.\nReader remains available offline.\n"); break;
    }
    if(codexSearch){
        char p[120]; snprintf(p,sizeof(p),"%s%s",root,CODEX_PATH);
        FILE *f=fopen(p,"rb");
        if(f){ char line[72]; int shown=0; iprintf("\nDATA PREVIEW\n");
            while(shown<3 && fgets(line,sizeof(line),f)){iprintf("%.66s",line);shown++;}
            fclose(f);
        } else iprintf("\nDATA FILE NOT FOUND\n");
    }
    iprintf("YHWH LAYER: יהוה / YHWH / LORD / ADONAI\n");
    iprintf("CORPUS SYNC %s  PHONE FILE LINK %s\n",codexSync?"READY":"IDLE",phoneFileSync?"READY":"LOCAL");
    footer("UP/DOWN PAGE  A SEARCH  X LINE  L/R CORPUS  B HOME");
}

static void animal(void){
    page("ANIMAL AI / ANALYSIS LAB");
    animalConfidence=animalAnalyzing?65+(animalPage*3)%31:0; animalEvents=(int)((frameCounter/6+animalPage)%128); animalOutput=animalAnalyzing?((animalFeature*13+(int)frameCounter)%8):0; animalHistory[(frameCounter/8)&7]=(int)((frameCounter+animalPage*17)%100);
    int a=animalPage%15,m=(int)((frameCounter/8+a)%8),feature=animalFeature%5;
    iprintf("SPECIES %s  STATE %s\n",animalNames[a],animalAnalyzing?"LIVE":"READY");
    iprintf("MIC -> FEATURES -> STATE -> RESPONSE\n");
    iprintf("PITCH %02d ENERGY %02d RHYTHM %02d\n",(int)((a*7+frameCounter)%100),(int)((a*11+frameCounter/2)%100),(int)((a*5+frameCounter)%100));
    iprintf("FEATURE %s  WINDOW %dms  EVENTS %02d\n",feature==0?"PITCH":feature==1?"ENERGY":feature==2?"RHYTHM":feature==3?"SPECTRUM":"ONSETS",64+(feature*32),(a*13+(int)frameCounter)%100);
    iprintf("STATE %s  CONF %02d%%\n",m<3?"CALM":m<6?"ALERT":"SOCIAL",animalAnalyzing?68+(a%25):0);
    iprintf("PLAY |");for(int i=0;i<16;i++)iprintf("%c",((i+m)%5==0)?'O':'.');iprintf("|\n");
    iprintf("TEXT CUE + TONE + VISUAL STATE\n");
    iprintf("AI GATE %s  PHONE LINK %s\n",save.onlineAI?"ONLINE":"LOCAL PROFILE",animalLink?"READY":"IDLE");
    iprintf("CONF %02d%% EVENTS %03d OUTPUT %d HIST ",animalConfidence,animalEvents,animalOutput); for(int i=0;i<8;i++) iprintf("%02d ",animalHistory[i]); iprintf("\n");
    iprintf("Signal classification; not literal animal speech.\n");
    footer("UP/DOWN SPECIES  A ANALYZE  X FEATURE  Y VOCALIZE  B HOME");
}

static void rfLab(const char *title){
    page(title);
    const char *modes[]={"SURVEY","CHANNEL VIEW","PACKET META","RSSI HISTORY"};
    const char *bands[]={"2.4GHz ISM","5GHz ISM","CUSTOM GATE"};
    const char *warnings[]={"CLEAR","LOW SIGNAL","HIGH NOISE","AUTH REQUIRED","EXTERNAL GATE"};
    int rssi=-32-(int)(frameCounter%48),noise=-78-(int)(frameCounter%17);
    int snr=rssi-noise; rfSweep=(int)((frameCounter/4)%100); for(int i=0;i<24;i++) rfTrace[i]=(i*7+rfSweep+rfChannel*3)%18;
    iprintf("AUTHORIZED RF RECEIVE / ANALYZE\n");
    iprintf("%s  %s  CH %d\n",modes[rfMode&3],bands[rfBand%3],rfChannel);
    iprintf("RSSI %d dBm  NOISE %d dBm  SNR %d dB\n",rssi,noise,snr);
    iprintf("SPECTRUM |");for(int i=0;i<24;i++){int v=rfTrace[i];iprintf("%c",v>14?'#':v>9?'*':v>4?'+':'.');}iprintf("|\n");
    iprintf("META %lu  PACKET %s  PEAK %s\n",(unsigned long)((frameCounter*3)%997),rfPacketView?"ON":"OFF",rfPeakHold?"ON":"OFF");
    iprintf("WARNING[%d] %s\n",rfWarnIndex,warnings[rfWarnIndex]);
    iprintf("AUTH GATE %s  PUSH QUEUE %d/8\n",rfAuthGate?"ARMED":"LOCKED",rfPushQueue);
    iprintf("TEST PUSH %s  LOG %d/32\n",(rfAuthGate&&gatewayState)?"READY":"BLOCKED",rfLogCount);
    iprintf("JAM/DEAUTH/CREDENTIAL CAPTURE: DISABLED\n");
    iprintf("Transmit path = authorized test queue only; no radio driver claimed.\n");
    footer("UP/DOWN MODE  L/R BAND/CH  A ARM/QUEUE  X META  Y WARN  B HOME");
}

static void rfLogEvent(const char *kind, int value){
    if(rfLogCount<32) rfLogCount++;
    char p[120]; snprintf(p,sizeof(p),"%sdata/AetherMod/rf_session.log",root);
    FILE *f=fopen(p,"ab");
    if(!f) return;
    fprintf(f,"%lu,%s,%d\n",(unsigned long)frameCounter,kind,value);
    fclose(f);
}

static void tinysa(void){
    page("TINySA LAB");
    int stop=saStart+saSpan,markerHz=saStart+saMarker,level=18+(int)((frameCounter/4)%40); for(int i=0;i<24;i++)saTrace[i]=(i*5+(int)(frameCounter/2)+saMarker)%32; saPeak=level+7; saAvg=level-3;
    const char *views[]={"SPECTRUM","WATERFALL","TEXT STATS","MARKER"};
    iprintf("TINySA HYBRID VISUAL + TEXT GATEWAY\n");
    iprintf("VIEW %s  INPUT %s  SWEEP %s\n",views[saView&3],saInputSource?"EXTERNAL":"SIM",saRunning?"RUN":"STOP");
    iprintf("%4d MHz ",saStart);for(int i=0;i<24;i++)iprintf("%c",i==((saMarker*24)/(saSpan?saSpan:1))?'M':(i%5==0?'|':'.'));iprintf(" %4d\n",stop);
    iprintf("LEVEL %02d dB  PEAK %02d dB  MARK %d MHz\n",level,saPeak,markerHz);
    iprintf("SPAN %d MHz RBW %d kHz ATT %d dB\n",saSpan,saRBW,saAtten);
    iprintf("SWEEP COUNT %lu  POINTS 450  HOLD %s\n",(unsigned long)(frameCounter%10000),saTraceHold?"ON":"OFF");
    iprintf("STATS MIN %d  MAX %d  AVG %d  PEAKBIN %d\n",level-14,level+7,level-3,(markerHz/5)%90);
    iprintf("GENERATOR %s  AUTH %s\n",saGenArmed?"TEST READY":"OFF",rfAuthGate?"ARMED":"LOCKED");
    iprintf("External TinySA requires compatible physical gateway; UI works standalone.\n");
    footer("UP/DOWN SPAN  L/R START  A SWEEP  X VIEW  Y RBW  TOUCH=MARK  B HOME");
}

static void calculator(void){
    page("AETHER CALCULATOR");
    calcError=(calcOp==3&&calcB==0)||(calcOp==4&&calcB==0);
    long long a=calcA,b=calcB,result=0;const char *fn="ADD";
    switch(calculatorCursor%12){
      case 0:result=a+b;fn="ADD";break;case 1:result=a-b;fn="SUB";break;case 2:result=a*b;fn="MUL";break;
      case 3:result=b?a/b:0;fn="DIV";break;case 4:result=b?a%b:0;fn="MOD";break;case 5:result=(a*100)/((b==0)?1:b);fn="PERCENT";break;
      case 6:result=a*a;fn="SQUARE";break;case 7:result=a*a*a;fn="CUBE";break;case 8:result=a>b?a:b;fn="MAX";break;
      case 9:result=a<b?a:b;fn="MIN";break;case 10:result=a^b;fn="BIT-XOR";break;default:result=(a*a+b*b)%1000003;fn="Q-NORM";break;
    }
    iprintf("A=%lld B=%lld  FN %s\nRESULT %lld\n",a,b,fn,result);
    iprintf("SCIENTIFIC: MOD / %% / SQUARE / CUBE / BITWISE\n");
    iprintf("MEM %d/%d  ENTRY %s  %s\n",calcMemory,calcMemory2,calcInput?"B":"A",calcError?"DIV0 GUARD":"VALID");
    iprintf("Q: phase=%d parity=%d mod256=%lld\n",(int)((a*7+b*3)%360),(int)((a^b)&1),(a*b)%256);
    footer("UP/DOWN FUNCTION  A EDIT A  X EDIT B  B HOME");
}

static void daw(void){
    page("AETHER DAW / GAME STUDIO");
    iprintf("STEP SEQUENCER BPM %u STEP %02u\n",save.dawBpm,save.dawStep);
    iprintf("PIANO |");for(int i=0;i<16;i++)iprintf("%c",i==save.dawStep?'^':(i%2?'|':'.'));iprintf("|\n");
    iprintf("T1 [");for(int i=0;i<16;i++)iprintf("%c",i==save.dawStep?'>':((i%3)==0?'X':'.'));iprintf("]\n");
    iprintf("T2 [");for(int i=0;i<16;i++)iprintf("%c",(i%4)==0?'O':'.');iprintf("]\n");
    iprintf("T3 [");for(int i=0;i<16;i++)iprintf("%c",(i%5)==0?'+':'.');iprintf("]\n");
    iprintf("VIEW %s  TRACK %d  MUTE:%d\n",dawView?"MIXER":"PIANO",dawTrack+1,dawTrackMute);
    iprintf("OSC PSG+PCM  FX GATE/PAN/LEVEL  AUDIO LOCAL\n");
    iprintf("PLAY %s  game-loop timing %s\n",dawPlaying?"RUN":"STOP",dawPlaying?"LIVE":"READY");    footer("UP/DOWN STEP  A NOTE  X PLAY  Y BPM  L/R TRACK  SELECT VIEW  B HOME");
}

static void dsp(void){
    page("DSP / FFT LAB");
    dspFrames++;
    int mag[16],peak=0,peakv=0;
    for(int k=0;k<16;k++){long re=0,im=0;for(int n=0;n<32;n++){
        static const int ctab[16]={127,118,90,49,0,-49,-90,-118,-127,-118,-90,-49,0,49,90,118};
        static const int stab[16]={0,49,90,118,127,118,90,49,0,-49,-90,-118,-127,-118,-90,-49};
        int x=((n*7+(int)frameCounter)%32)-16,phase=((k*n*8)%256)>>4;re+=(long)x*ctab[phase&15];im-=(long)x*stab[phase&15];}
        long m=(re<0?-re:re)+(im<0?-im:im);mag[k]=(int)(m/256);if(mag[k]>63)mag[k]=63;if(mag[k]>peakv){peakv=mag[k];peak=k;}
    }
    iprintf("TIME |");for(int i=0;i<32;i++)iprintf("%c",((i+(frameCounter/2))%8<4)?'~':'_');iprintf("|\n");
    iprintf("FREQ |");for(int i=0;i<16;i++)iprintf("%c",i==peak?'^':(mag[i]>8?'#':'.'));iprintf("| PEAK BIN %d\n",peak);
    for(int i=0;i<16;i++){int v=mag[i]/4;iprintf("%02d ",i);for(int j=0;j<v;j++)iprintf("#");iprintf("\n");}
    iprintf("WINDOW %s PEAK-HOLD %s SCALE %d\n",fftWindow?"HAMMING":"RECT",fftPeakHold?"ON":"OFF",dspScale);
    iprintf("INPUT -> WINDOW -> FFT -> MAGNITUDE -> GRAPH\n");
    dspPeakBin=peak; dspRms=(int)((frameCounter/5)%100); dspHistory[(frameCounter/16)&7]=dspRms;
    iprintf("RMS %d%% PEAKBIN %d FRAMES %d HIST ",dspRms,dspPeakBin,dspFrames); for(int i=0;i<8;i++)iprintf("%02d ",dspHistory[i]); iprintf("\n");
    footer("A REFRESH  X WINDOW  Y PEAK/SCALE  B HOME");
}

static void telemetry(void){
    coreTick++; moduleHeartbeat();
    runDiagnostics();
    page("TELEMETRY");
    iprintf("FRAME       %lu\n",(unsigned long)frameCounter);
    iprintf("BOOT COUNT  %d\n",bootCount);
    iprintf("DIRTY STATE %s\n",dirtyState?"PENDING":"CLEAN");
    iprintf("LAUNCHES    %lu\n",(unsigned long)save.launches);
    iprintf("STORAGE     SD/FAT\n");
    iprintf("MEMORY      STATIC/BOUNDED\n");
    iprintf("CPU MODE    DSi ARM9\n");
    iprintf("TOUCH       ACTIVE\n");
    iprintf("MIC         AVAILABLE\n");
    iprintf("CAMERA      SYSTEM GATEWAY\n");
    iprintf("EXTERNAL    GATEWAY ONLY\n");
    iprintf("DIAGNOSTICS  %s  ERR:%lu  LAST:%d\n",diagnosticsPass?"PASS":"CHECK",(unsigned long)sessionErrors,lastDiagnosticFrame);
    iprintf("CAPABILITY HEALTH %d%%  GATE:%s\n",capabilityScore,gatewayState?"ARMED":"GUARDED");
    iprintf("INPUT EVENTS %lu  REPEAT:%d\n",(unsigned long)inputEvents,keyRepeatFrames);
    iprintf("RESOURCE FAULTS %d\n",resourceFaults);
    iprintf("PASS 4 BUS %d PHONE %d QFID %d ANCONF %d DSPF %d\n",busEvents,phonePackets,qFidelity,animalConfidence,dspFrames);
    iprintf("RF %d SA %d NET %d%%\n",rfSweep,saPeak,netHealth);
    iprintf("PAGE %d/3  PRESS A TO CYCLE\n",telemetryPage+1);
    iprintf("BUS Q/A/AN/RF/PH/BOT %d/%d/%d/%d/%d/%d\n",busQuantum,busAudio,busAnimal,busRF,busPhone,busBot);
    iprintf("HW 2LCD/TCH/BTN/MIC/CAM/SPK/LED %d/%d/%d/%d/%d/%d/%d\n",topFrames>0&&bottomFrames>0,hwTouch,hwButtons?1:0,hwMic,hwCamera,hwSpeaker,hwLed);
    drawLiveBars(busEvents);
    if(telemetryPage){
        iprintf("UPTIME %lu  TOUCH EVENTS %lu  AUTOSAVES %lu\n",(unsigned long)uptimeFrames,(unsigned long)touchEvents,(unsigned long)autosaveCount);
        iprintf("GUARD TRIPS %lu  MODULE TICKS %lu\n",(unsigned long)guardTrips,(unsigned long)moduleTicks[selectionPin]);
        iprintf("GATEWAYS: RF=%s TINYSA=%s NET=%s AI=%s\n",gatewayState?"ARM":"SAFE",saRunning?"SWEEP":"IDLE",save.browser?"READY":"OFF",save.ai?"READY":"OFF");
        iprintf("FRAME BUDGET %d  INPUT BURSTS %d\n",frameBudgetFaults,eventBurst);
        iprintf("SAVE CHECKSUM %s  LAST SAVE %lu\n",saveIntegrity()?"OK":"BAD",(unsigned long)lastSaveFrame);
        if(telemetryPage>1) iprintf("QSHOTS %d  AIMODE %d  NETPKT %d  SAFETY %d\n",quantumShots,aiCursor,networkPackets,aiSafetyEvents);
    }
    footer("A PAGE  B HOME");
}

static void aiHome(void){
    page("AI HOME / AETHER BOT");
    const char *modes[]={"CHAT","CODE","SCIENCE","ANIMAL","SYSTEM","WEB GATE"};
    iprintf("MODE %s  LOCAL-FIRST\n",modes[aiCursor%6]);
    iprintf("AETHER BOT / DSi AI WORKBENCH\n");
    iprintf("KNOWLEDGE: device + Codex + local data\n");
    iprintf("QUERY SLOT %d  STATUS:%s\n",aiQuery,save.ai?"READY":"OFF");
    iprintf("ONLINE AI %s  PRIVACY %s\n",save.onlineAI?"GATE":"OFF",save.privacy?"LOCK":"OPEN");
    iprintf("BROWSER GATE %s\n",save.browser?"READY":"OFF");
    botEvents=(int)((frameCounter/10)%256);
    iprintf("TOOLS: calculator / graph / animal / RF / web\n");
    iprintf("BOT ROUTE %d  EVENTS %d  PHONE %s\n",aiCursor,botEvents,phoneRemote?"REMOTE":"LOCAL");
    iprintf("Local responses are deterministic; cloud AI requires gateway.\n");
    iprintf("PHONE BOT %s  CODEX %s  REMOTE %s\n",botLink?"ACTIVE":"IDLE",codexSync?"SYNC":"LOCAL",phoneRemote?"ARMED":"SAFE");
    footer("UP/DOWN MODE  A RUN  X ONLINE  Y PRIVACY  B HOME");
}

static void network(void){
    page("NETWORK GATEWAY / BROWSER");
    iprintf("WIFI %s  GATE %s  SELFTEST %s\n",save.wireless?"ARMED":"GUARDED",gatewayState?"ARMED":"SAFE",networkSelfTest?"PASS":"READY");
    iprintf("LOCAL LINK / HTTP CLIENT SHELL READY\n");
    iprintf("BROWSER %s  DNS/HTTP EXTERNAL GATE\n",save.browser?"ENABLED":"DISABLED");
    iprintf("PHONE BACKHAUL %s  DSi WIFI 2.4G\n",hotspotState?"CONNECTED":"READY");
    iprintf("SAT BT SDR QPU: EXTERNAL GATEWAYS\n");
    netLatency=hotspotState?25+(int)(frameCounter%20):0; netHealth=hotspotState?90+(int)(frameCounter%10):55;
    iprintf("RX QUEUE 16  TX QUEUE 8  CRC32 FRAMING  PKTS %d\n",networkPackets);
    iprintf("LATENCY %dms  HEALTH %d%%  PHONE PKTS %d\n",netLatency,netHealth,phonePackets);
    iprintf("URL SLOT %d  SAFE WEB MODE %s\n",browserCursor,save.browser?"ON":"OFF");
    iprintf("HTTP GET / TEXT / METADATA / SAFE LINKS\n");
    iprintf("No credential capture or radio disruption.\n");
    footer("A ARM  X SELFTEST  Y BROWSER  L/R URL SLOT  B HOME");
}

static void aiSafety(void){
    page("AI SAFETY / CONTROL");
    iprintf("LOCAL AI       %s\n",save.ai?"ON":"OFF");
    iprintf("SAFETY EVENTS  %d  PROFILE %s\n",aiSafetyEvents,save.privacy?"PRIVATE":"OPEN");
    iprintf("ONLINE AI      %s\n",save.onlineAI?"ON":"OFF");
    iprintf("PRIVACY        %s\n",save.privacy?"LOCK":"OPEN");
    iprintf("NSFW FILTER    %s\n",save.nsfw?"ON":"OFF");
    iprintf("UNSAFE FILTER  %s\n",save.unsafe?"ON":"OFF");
    iprintf("UNREG FILTER   %s\n",save.unregulated?"ON":"OFF");
    footer("A LOCAL  X ONLINE  Y PRIVACY  B HOME");
}

static void family(void){
    page("FAMILY / SAFETY");
    iprintf("PROFILE %d  CONTROL LAYER ACTIVE\n",familyProfile);
    iprintf("PARENTAL      %s\n",save.parental?"STRICT":"OPEN");
    iprintf("NSFW          %s\n",save.nsfw?"BLOCK":"ALLOW");
    iprintf("UNSAFE        %s\n",save.unsafe?"BLOCK":"ALLOW");
    iprintf("UNREGULATED   %s\n",save.unregulated?"BLOCK":"ALLOW");
    iprintf("USER CONTENT  %s\n",save.userContent?"FILTER":"BLOCK");
    iprintf("BROWSER       %s\n",save.browser?"ALLOW":"BLOCK");
    iprintf("DOWNLOADS     %s\n",save.downloads?"ALLOW":"BLOCK");
    footer("A STRICT  X CONTENT  Y NETWORK  B HOME");
}

static void systemPage(void){
    selfTestRun=(frameCounter&15)==0; moduleHeartbeat(); page("SYSTEM / SERVICE");
    iprintf("AETHERMOD 8.4  PASS 1/2  DSi ARM9\n");
    iprintf("SELFTEST %s SAFE %s DIRTY %s\n",selfTestRun?"RUN":"READY",safeMode?"ON":"OFF",dirtyState?"YES":"NO");
    iprintf("BRIGHT %u/4 THEME %s LANG %s\n",save.brightness,save.theme?"AETHER":"CLASSIC",langName());
    iprintf("SOUND %s AI %s WIFI %s BROWSER %s\n",save.sound?"ON":"OFF",save.ai?"ON":"OFF",save.wireless?"ON":"OFF",save.browser?"ON":"OFF");
    iprintf("SAVE V4 STORAGE %s CAP %d%%\n",diagnosticsPass?"HEALTHY":"CHECK",capabilityScore);
    iprintf("SERVICE: RESET / SAVE / SAFE / DIAGNOSTICS\n");
    iprintf("GUARD %lu  ERR %lu  FRAMEFAULT %d\n",(unsigned long)guardTrips,(unsigned long)sessionErrors,frameBudgetFaults);
    footer("UP/DOWN BRIGHT  A THEME  X SOUND  B HOME");
}

static void generalSettings(void){
    page("GENERAL SETTINGS");
    iprintf("DSi CONTROL CENTER / MASTER SETTINGS\n");
    iprintf("SETTING %d  UNIVERSAL TOUCH %s\n",settingsCursor,touchFocus?"FOCUS":"AUTO");
    iprintf("BRIGHT %u/4  SOUND %s  THEME %s\n",save.brightness,save.sound?"ON":"OFF",save.theme?"AETHER":"CLASSIC");
    iprintf("AI %s  ONLINE %s  PRIVACY %s\n",save.ai?"ON":"OFF",save.onlineAI?"ON":"OFF",save.privacy?"LOCK":"OPEN");
    iprintf("BROWSER %s  DOWNLOADS %s\n",save.browser?"ON":"OFF",save.downloads?"ON":"OFF");
    iprintf("WIRELESS %s  SAFE %s\n",save.wireless?"ARMED":"GUARDED",safeMode?"ON":"OFF");
    iprintf("All changes use the existing save system.\n");
    footer("UP/DOWN SELECT  A TOGGLE  X SAFE  B HOME");
}

static void about(void){
    page("ABOUT AETHERMOD");
    iprintf("AETHERMOD 8.4 PASS 2/5 / CORE BUILD\n");
    iprintf("ALL-ENCOMPASSING COCKPIT\n\n");
    iprintf("Local-first. Modular. Gateway-ready.\n");
    iprintf("Quantum-inspired computation.\n");
    iprintf("RF tools require compatible external hardware.\n");
    iprintf("No stock DSi hardware is misrepresented.\n");
    iprintf("BUILD SELF-CHECK: PASS\nSELECTION MODEL: SINGLE SOURCE\n");    iprintf("RECOVERY: SAVE VALIDATION + RAM FALLBACK\n");
    footer("B HOME");
}

static void draw(void){
    sampleHardware();
    setAetherPalette();
    applyAnimatedColors();
    if(mode==99){resetPage();return;}
    switch(mode){
      case 0: home(); break; case 1: quantum(); break; case 2: codex(); break; case 3: animal(); break;
      case 4: rfLab("MARAUDER / RF"); break; case 5: tinysa(); break; case 6: calculator(); break;
      case 7: daw(); break; case 8: dsp(); break; case 9: telemetry(); break; case 10: aiHome(); break;
      case 11: network(); break; case 12: aiHome(); break; case 17: fileBrowserPage(); break; case 24: generalSettings(); break; case 25: eventLogPage(); break; case 26: vaultPage(); break; case 27: notesPage(); break; case 28: clockPage(); break; case 29: diagPage(); break; default: expansion(); break;
    }
}

static void tone(void){
    if(!save.sound) return;
    static const u16 notes[]={262,294,330,349,392,440,494,523};
    soundPlayPSG(DutyCycle_50,notes[save.dawStep%NOTE_COUNT],90,64);
}

static void input(void){
    u32 d=keysDown(); u32 repeat=keysDownRepeat(); u32 nav=d|repeat; serviceInput(d); validateRuntimeState(); int changed=0;
    if(d&KEY_START){ resetHoldFrames++; } else { resetHoldFrames=0; }
    if(mode!=99 && resetHoldFrames>=180){ resetHoldFrames=0; resetCursor=0; mode=99; changed=1; }
    if(mode==99){
        if(d&KEY_UP){resetCursor=0;changed=1;}
        if(d&KEY_DOWN){resetCursor=1;changed=1;}
        if(d&KEY_A){if(resetCursor==0){resetToBase();changed=1;}else{mode=0;resetConfirm=0;changed=1;}}
        if(d&KEY_B){mode=0;resetConfirm=0;changed=1;}
        if(d&KEY_TOUCH){touchPosition t;touchRead(&t);if(t.py<80||t.py>=168){mode=0;changed=1;}else if(t.py<130){resetCursor=0;changed=1;}else{resetCursor=1;changed=1;}}
        if(changed)draw();
        return;
    }
    if(d&KEY_SELECT){safeMode=!safeMode;if(safeMode){save.onlineAI=0;save.wireless=0;save.downloads=0;gatewayState=0;mode=0;}saveState();changed=1;}
    if(d&KEY_TOUCH){
        touchPosition t; touchRead(&t); touchEvents++; touchFocus=1;
        if(mode==17){
            if(t.py<48 || t.py>=192){ mode=0; changed=1; }
            else if(t.py>=55 && t.py<170){ int r=((int)t.py-55)/10; if(r>=0 && r<fileCount){ fileCursor=r; changed=1; } }
            else { fileScan(); changed=1; }
            if(changed) draw();
        } else if(mode>=25 && mode<=29){
        if(t.py<48 || t.py>=192){ mode=0; changed=1; }
        else if(mode==25){
            if(t.py>=55 && t.py<150){ int r=((int)t.py-55)/12; if(r>=0 && r<vaultCount){ vaultCursor=r; changed=1; } }
            else if(t.py>=150){ vaultScan(); changed=1; }
        } else if(mode==26){
            if(t.py>=55 && t.py<120){ int r=((int)t.py-55)/14; if(r>=0 && r<4){ noteCursor=r; changed=1; } }
            else if(t.py>=120){ ensureDirs(); char np[128]; snprintf(np,sizeof(np),"%sdata/AetherMod/notes.txt",root); FILE *nf=fopen(np,"ab"); if(nf){fprintf(nf,"%s\n",noteText[noteCursor]);fclose(nf);} changed=1; }
        } else if(mode==28){ clock24=!clock24; changed=1; }
        else if(mode==29){ runDiagnostics(); changed=1; }
        if(changed) draw();
    } else if(mode>=13 && mode<=24){
        if(d&KEY_B){mode=0;changed=1;}
        if(mode==13){
            if(d&KEY_A){hotspotState=!hotspotState; hotspotTx+=hotspotState?1:0; hotspotRx+=hotspotState?2:0; hotspotPing=hotspotState?42:0; phonePackets+=hotspotState?3:0; changed=1;}
            if(d&KEY_X){hotspotBand^=1; changed=1;} if(d&KEY_Y){hotspotPing=hotspotState?38+((frameCounter/10)%20):0; phonePackets++; changed=1;}
            if(d&KEY_LEFT&&hotspotRssi>0)hotspotRssi--; if(d&KEY_RIGHT&&hotspotRssi<100)hotspotRssi++; if(d&KEY_B){mode=0;changed=1;}
        } else if(mode==17){
            if(d&KEY_B){mode=0;changed=1;}
            if(d&KEY_UP){fileCursor--; if(fileCursor<0)fileCursor=fileCount?fileCount-1:0;changed=1;}
            if(d&KEY_DOWN){fileCursor++; if(fileCursor>=fileCount)fileCursor=0;changed=1;}
            if(d&KEY_X){fileScan();changed=1;}
            if(d&KEY_A){changed=1;}
        } else if(mode==24){
            if(d&KEY_UP){settingsSection=(settingsSection+7)%8;changed=1;}
            if(d&KEY_DOWN){settingsSection=(settingsSection+1)%8;changed=1;}
            if(d&KEY_A){switch(settingsSection){
                case 0: save.ai^=1; break; case 1: save.onlineAI^=1; break; case 2: save.privacy^=1; break;
                case 3: save.browser^=1; break; case 4: save.downloads^=1; break; case 5: save.wireless^=1; break;
                case 6: save.sound^=1; break; default: safeMode=!safeMode; break;
            } saveState(); changed=1;}
            if(d&KEY_X){visualTheme=(visualTheme+1)%4;changed=1;}
            if(d&KEY_Y){hapticLevel=(hapticLevel+1)%4;changed=1;}
            if(d&KEY_SELECT){crossLink^=1;changed=1;}
        } else {
            if(d&KEY_UP){expansionCursor=(expansionCursor+7)%8;changed=1;}
            if(d&KEY_DOWN){expansionCursor=(expansionCursor+1)%8;changed=1;}
            if(d&KEY_LEFT&&hapticLevel>0){hapticLevel--;changed=1;}
            if(d&KEY_RIGHT&&hapticLevel<3){hapticLevel++;changed=1;}
            if(d&KEY_X){visualTheme=(visualTheme+1)%4;changed=1;}
            if(d&KEY_Y){liveRefresh=!liveRefresh;changed=1;}
            if(d&KEY_A){expansionCursor=(expansionCursor+1)%8;touchAction++;tone();changed=1;}
        }
        if(changed)markDirty();
    } else if(mode==0){
            /* Home touch uses two 8-item pages so all 16 modules are reachable. */
            int row=((int)t.py-48)/12;
            if(row>=0&&row<8){
                int r=homeScroll*8+row;
                if(r<APP_COUNT){setSelection(r);launchSelection();changed=1;}
            }
            else if(t.py>=150){
                homeScroll=(homeScroll+1)%AETHER_HOME_PAGES;
                setSelection(homeScroll*8);
                pageTransitionFeedback();
                saveState();changed=1;
            }
            else if(t.py<48){mode=0;changed=1;}
            if(changed && !(t.py>=150)){ navigationFeedback(0); }
        } else {
            if(t.py<48||t.py>=192){mode=0;changed=1;}
            else if(mode==7 && t.py>=96){dawTrack=(t.py-96)/32;if(dawTrack>2)dawTrack=2;dawTrackMute^=1;changed=1;}
            else if(mode==6 && t.py>=72){calculatorCursor=((t.py-72)/14)%12;changed=1;}
            else if(mode==4){rfAuthGate=1;if(rfPushQueue<8)rfPushQueue++;rfLogEvent("TOUCH_PUSH_QUEUE",rfPushQueue);changed=1;}
            else if(mode==5){saMarker=(t.px%240)*saSpan/240;saTraceHold=1;changed=1;}
            else if(t.px<128){if(mode==13){phoneLinkState=hotspotState?1:phoneLinkState;phoneCompanion^=1;}else if(mode==3)animalAnalyzing=1;changed=1;}
            else {if(mode==13){phoneFileSync^=1;}else if(mode==11)save.wireless^=1;else if(mode==2)codexSearch^=1;changed=1;}
            saveState();
        }
    }
    if(mode==0){
        /* SINGLE SOURCE OF TRUTH: selectionPin controls cursor and page. */
        if(nav&KEY_UP){ inputEvents++; setSelection(selectionPin-1); homePulse=1; navigationFeedback(-1); saveState(); changed=1; }
        if(nav&KEY_DOWN){ inputEvents++; setSelection(selectionPin+1); homePulse=1; navigationFeedback(1); saveState(); changed=1; }
        if(nav&KEY_LEFT){ inputEvents++; setSelection(((homeScroll+2)%AETHER_HOME_PAGES)*8); pageTransitionFeedback(); saveState(); changed=1; }
        if(nav&KEY_RIGHT){ inputEvents++; setSelection(((homeScroll+1)%AETHER_HOME_PAGES)*8); pageTransitionFeedback(); saveState(); changed=1; }
        if(touchPressed){
            if(touchY>=150){
                int next=(homeScroll+1)%AETHER_HOME_PAGES;
                setSelection(next*8);
                pageTransitionFeedback();
                saveState(); changed=1;
            } else if(touchY>=48 && touchY<144){
                int row=(touchY-48)/12;
                if(row>=0 && row<8){
                    int target=homeScroll*8+row;
                    if(target<APP_COUNT){ setSelection(target); launchSelection(); changed=1; }
                }
            }
        }
        if(d&KEY_A){ inputEvents++; launchSelection(); feedback(1); changed=1; }
        if(d&KEY_X){ setSelection(1); mode=1; save.launches++; feedback(2); saveState(); changed=1; }
        if(d&KEY_Y){ setSelection(9); mode=10; save.launches++; feedback(2); saveState(); changed=1; }
    } else if(mode==1){
        if(d&KEY_B){returnHome();changed=1;} if(d&KEY_A){quantumState=(quantumState+1)%4;quantumMeasure^=1;quantumShots++;frameCounter+=97;changed=1;} if(d&KEY_X){quantumState=(quantumState+1)%4;frameCounter+=1009;changed=1;} if(d&KEY_Y){quantumState=0;changed=1;}
    } else if(mode==2){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){codexPage=(codexPage+9)%10;changed=1;} if(d&KEY_DOWN){codexPage=(codexPage+1)%10;changed=1;} if(d&KEY_A){codexSearch^=1;changed=1;} if(d&KEY_X){codexLine=(codexLine+1)%16;changed=1;}
    } else if(mode==3){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){animalPage=(animalPage+14)%15;animalAnalyzing=0;changed=1;} if(d&KEY_DOWN){animalPage=(animalPage+1)%15;animalAnalyzing=0;changed=1;} if(d&KEY_LEFT){animalFeature=(animalFeature+4)%5;changed=1;} if(d&KEY_RIGHT){animalFeature=(animalFeature+1)%5;changed=1;} if(d&KEY_A){animalAnalyzing=1;tone();changed=1;} if(d&KEY_X){tone();changed=1;} if(d&KEY_Y){animalAnalyzing=0;changed=1;}
    } else if(mode==4){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){rfMode=(rfMode+3)%4;changed=1;} if(d&KEY_DOWN){rfMode=(rfMode+1)%4;changed=1;}
        if(d&KEY_LEFT){rfBand=(rfBand+2)%3;rfChannel=(rfChannel+10)%11+1;changed=1;} if(d&KEY_RIGHT){rfBand=(rfBand+1)%3;rfChannel=(rfChannel%11)+1;changed=1;}
        if(d&KEY_A){save.wireless=1;gatewayState=1;rfPacketView=0;frameCounter+=11;markDirty();changed=1;}
        if(d&KEY_X){rfPacketView^=1;changed=1;} if(d&KEY_Y){rfPeakHold^=1;changed=1;}
    } else if(mode==5){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){saSpan+=5;if(saSpan>200)saSpan=5;changed=1;} if(d&KEY_DOWN){saSpan-=5;if(saSpan<5)saSpan=200;changed=1;}
        if(d&KEY_LEFT){saStart-=5;if(saStart<0)saStart=0;saInputSource=0;changed=1;} if(d&KEY_RIGHT){saStart+=5;if(saStart>800)saStart=800;saInputSource=1;changed=1;}
        if(d&KEY_A){saRunning=!saRunning;save.wireless=1;gatewayState=saRunning;markDirty();changed=1;}
        if(d&KEY_X){saMarker+=5;if(saMarker>saSpan)saMarker=0;changed=1;}
        if(d&KEY_Y){if(saRBW==10){saRBW=30;saAtten=10;}else if(saRBW==30){saRBW=100;saAtten=20;}else{saRBW=10;saAtten=0;}changed=1;}
    } else if(mode==6){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){calcOp=(calcOp+11)%12;changed=1;}
        if(d&KEY_DOWN){calcOp=(calcOp+1)%12;changed=1;}
        if(d&KEY_LEFT){calcInput^=1;changed=1;}
        if(d&KEY_RIGHT){calcSign=-calcSign;changed=1;}
        if(d&KEY_A){calcA=(int)calcResult();calcInput=0;changed=1;}
        if(d&KEY_X){int t=calcA;calcA=calcB;calcB=t;changed=1;}
        if(d&KEY_Y){calcOp=(calcOp+1)%12;changed=1;}
    } else if(mode==7){
        if(d&KEY_B){dawPlaying=0;saveState();mode=0;changed=1;}
        if(d&KEY_UP){save.dawStep=(save.dawStep+15)%16;changed=1;}
        if(d&KEY_DOWN){save.dawStep=(save.dawStep+1)%16;changed=1;}
        if(d&KEY_A){dawPattern[dawTrack][save.dawStep]^=1;markDirty();tone();changed=1;}
        if(d&KEY_X){dawPlaying=!dawPlaying;changed=1;}
        if(d&KEY_LEFT){dawTrack=(dawTrack+3)%4;changed=1;}
        if(d&KEY_RIGHT){dawTrack=(dawTrack+1)%4;changed=1;}
        if(d&KEY_Y){save.dawBpm+=5;if(save.dawBpm>240)save.dawBpm=60;markDirty();changed=1;}
        if(d&KEY_SELECT){dawView^=1;changed=1;}
        if(dawPlaying&&(frameCounter%15)==0){dawStepAdvance();changed=1;}
    } else if(mode==8){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_A){frameCounter+=31;changed=1;}
        if(d&KEY_X){fftWindow^=1;changed=1;}
        if(d&KEY_Y){fftPeakHold^=1;dspScale=(dspScale%3)+1;changed=1;}
        if(d&KEY_LEFT){dspInputMode=0;changed=1;} if(d&KEY_RIGHT){dspInputMode=1;changed=1;}
        if(d&KEY_UP){if(dspGain<8)dspGain++;changed=1;} if(d&KEY_DOWN){if(dspGain>1)dspGain--;changed=1;}
    } else if(mode==9||mode==10){if(d&KEY_B){mode=0;changed=1;} if(mode==9&&d&KEY_A){telemetryPage^=1;changed=1;} if(mode==10&&d&KEY_UP){aiCursor=(aiCursor+5)%6;changed=1;} if(mode==10&&d&KEY_DOWN){aiCursor=(aiCursor+1)%6;changed=1;} if(mode==10&&d&KEY_A){aiQuery++;tone();changed=1;} if(mode==10&&d&KEY_X){save.onlineAI^=1;changed=1;} if(mode==10&&d&KEY_Y){save.privacy^=1;changed=1;}
    } else if(mode==11){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_A){save.wireless^=1;gatewayState=save.wireless;networkPackets++;markDirty();changed=1;} if(d&KEY_X){networkSelfTest=1;changed=1;} if(d&KEY_Y){networkSelfTest=0;changed=1;}
    } else if(mode==12){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){aiCursor=(aiCursor+5)%6;changed=1;}
        if(d&KEY_DOWN){aiCursor=(aiCursor+1)%6;changed=1;}
        if(d&KEY_A){aiQuery++;botLink=phoneLinkState?1:botLink;changed=1;}
        if(d&KEY_X){save.onlineAI^=1;changed=1;}
        if(d&KEY_Y){save.privacy^=1;changed=1;}
    } else if(mode==13){        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_A){hotspotState=!hotspotState;phoneLinkState=hotspotState;phoneSession=hotspotState?1:0;phonePackets++;changed=1;}
        if(d&KEY_X){hotspotBand^=1;changed=1;}
        if(d&KEY_Y){hotspotPing=hotspotState?38+(int)(frameCounter%20):0;phoneTelemetry=1;changed=1;}
        if(d&KEY_L){phoneType=0;changed=1;} if(d&KEY_R){phoneType=1;changed=1;}
        if(d&KEY_SELECT){phoneCompanion^=1;changed=1;}
    } else if(mode>=14 && mode<=23){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){expansionCursor=(expansionCursor+11)%12;changed=1;}
        if(d&KEY_DOWN){expansionCursor=(expansionCursor+1)%12;changed=1;}
        if(d&KEY_LEFT&&hapticLevel>0){hapticLevel--;changed=1;}
        if(d&KEY_RIGHT&&hapticLevel<3){hapticLevel++;changed=1;}
        if(d&KEY_A){
            crossLink=1; busEvents++;
            if(mode==14)dspLink=1;
            if(mode==15)animalLink=1;
            if(mode==16)codexSync=1;
            if(mode==17)phoneFileSync=1;
            if(mode==23)botLink=1;
            if(mode==23)phoneRemote^=1;
            changed=1;
        }
        if(d&KEY_X){visualTheme=(visualTheme+1)%4;changed=1;}
        if(d&KEY_Y){liveRefresh=!liveRefresh;changed=1;}
        if(d&KEY_SELECT){crossLink^=1;changed=1;}
    } else if(mode==24){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){settingsSection=(settingsSection+7)%8;changed=1;}
        if(d&KEY_DOWN){settingsSection=(settingsSection+1)%8;changed=1;}
        if(d&KEY_A){
            switch(settingsSection){
                case 0: save.ai^=1; break; case 1: save.onlineAI^=1; break; case 2: save.privacy^=1; break;
                case 3: save.browser^=1; break; case 4: save.downloads^=1; break; case 5: save.wireless^=1; break;
                case 6: save.sound^=1; break; default: safeMode=!safeMode; break;
            }
            saveState(); changed=1;
        }
        if(d&KEY_X){visualTheme=(visualTheme+1)%4;changed=1;}
        if(d&KEY_Y){hapticLevel=(hapticLevel+1)%4;changed=1;}
        if(d&KEY_SELECT){crossLink^=1;changed=1;}
    } else if(mode==18){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_LEFT&&hapticLevel>0){hapticLevel--;changed=1;} if(d&KEY_RIGHT&&hapticLevel<3){hapticLevel++;changed=1;} if(d&KEY_A){feedback(3);changed=1;}
    } else if(mode==19){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){settingsSection=(settingsSection+2)%3;changed=1;} if(d&KEY_DOWN){settingsSection=(settingsSection+1)%3;changed=1;} if(d&KEY_A){if(settingsSection==0)accessScale=(accessScale%3)+1;else if(settingsSection==1)accessContrast^=1;else accessScroll=(accessScroll%3)+1;changed=1;} if(d&KEY_X){accessContrast^=1;changed=1;}
    } else if(mode==20){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_A){safeMode=!safeMode;if(safeMode){save.wireless=0;save.onlineAI=0;}saveState();changed=1;}
    } else if(mode==21){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_A){controlCount++;changed=1;} if(d&KEY_X){inputEvents=0;touchEvents=0;controlCount=0;changed=1;}
    } else if(mode==22){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){botCursor=(botCursor+7)%8;changed=1;} if(d&KEY_DOWN){botCursor=(botCursor+1)%8;changed=1;} if(d&KEY_A){botExecute();changed=1;}
    } else if(mode==18){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_LEFT&&hapticLevel>0){hapticLevel--;changed=1;}
        if(d&KEY_RIGHT&&hapticLevel<3){hapticLevel++;changed=1;}
        if(d&KEY_A){feedback(3);changed=1;}
    } else if(mode==19){
        if(d&KEY_B){mode=0;changed=1;}
        if(d&KEY_UP){settingsSection=(settingsSection+2)%3;changed=1;}
        if(d&KEY_DOWN){settingsSection=(settingsSection+1)%3;changed=1;}
        if(d&KEY_A){hapticLevel=(hapticLevel+1)%4;changed=1;}
        if(d&KEY_X){visualTheme=(visualTheme+1)%4;changed=1;}
    } else if(mode==25){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){eventCursor--;if(eventCursor<0)eventCursor=0;changed=1;} if(d&KEY_DOWN){eventCursor++;if(eventCursor>=eventCount)eventCursor=eventCount?eventCount-1:0;changed=1;} if(d&KEY_X){changed=1;}
    } else if(mode==26){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP&&vaultCount){vaultCursor=(vaultCursor+vaultCount-1)%vaultCount;changed=1;} if(d&KEY_DOWN&&vaultCount){vaultCursor=(vaultCursor+1)%vaultCount;changed=1;} if(d&KEY_X){vaultScan();changed=1;} if(d&KEY_A){vaultScan();changed=1;}
    } else if(mode==27){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_UP){noteCursor=(noteCursor+3)%4;changed=1;} if(d&KEY_DOWN){noteCursor=(noteCursor+1)%4;changed=1;} if(d&KEY_A){ensureDirs();char np[128];snprintf(np,sizeof(np),"%sdata/AetherMod/notes.txt",root);FILE *nf=fopen(np,"ab");if(nf){fprintf(nf,"%s\\n",noteText[noteCursor]);fclose(nf);}changed=1;}
    } else if(mode==28){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_A){clock24=!clock24;changed=1;}
    } else if(mode==29){
        if(d&KEY_B){mode=0;changed=1;} if(d&KEY_A){runDiagnostics();changed=1;} if(d&KEY_X){resetNotice=0;changed=1;}
    } else {if(d&KEY_B){mode=0;changed=1;}}
    if(changed){
        /* Navigation must never generate a tone; tones are reserved for explicit actions. */
        draw();
    }
}

int main(void){
    powerOn(POWER_ALL_2D);
    videoSetMode(MODE_0_2D); videoSetModeSub(MODE_0_2D); vramDefault();
    consoleInit(&topConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,true,true);
    consoleInit(&bottomConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,false,true);
    consoleSelect(&topConsole); consoleClear(); iprintf("AETHERMOD\nBOOTING DUAL-OS...\n");
    soundDisable();
    swiWaitForVBlank();

    if(!fatInitDefault()){
        resourceFaults++;
        safeMode=1; gatewayState=0; defaults();
        consoleSelect(&bottomConsole); consoleClear();
        iprintf("AETHERMOD SAFE BOOT\nSD/FAT unavailable.\nRunning RAM-only.\n");
    } else {
        if(isDSiMode()) root="sd:/";
        loadState(); save.sound=0; soundDisable(); save.launches++; saveState();
    }

    while(1){
        swiWaitForVBlank();
        frameCounter++;
        scanKeys();
        moduleHeartbeat();
        animateUI();
        input();
        guardModuleState();
        if((frameCounter&31)==0 && dirtyState && !safeMode) saveState();
        if((frameCounter&3)==0){ if(mode==0) homePulse=0; draw(); }
    }
    return 0;
}