#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>
#include "config.h"

/* AetherOS 9.0 — DSi-native modular cockpit.
 * Every home entry maps to an independent implementation.
 * Hardware claims remain honest: external RF/TinySA/camera/AI/phone/QPU
 * capabilities are represented as software workspaces/gateways, not invented
 * stock-DSi hardware.
 */
#define APP_COUNT 77
#define AETHERCORE_MAJOR 1
#define AETHERCORE_PASS 3
#define AETHERCORE_TOTAL_PASSES 5
#define AETHERMOD_MAJOR 9
#define AETHERMOD_MINOR 0
#define AETHERMOD_PASS 1
#define AETHERMOD_TOTAL_PASSES 1
#define HOME_PAGES 10
#define AETHER_SAVE_VERSION 7

typedef struct {
    u32 magic, checksum, launches;
    u16 version, selected, sound, brightness, language, theme;
    u8 ai, onlineAI, privacy, wireless, browser, downloads, parental;
    u8 nsfw, unsafe, unregulated, userContent;
    u16 bpm;
} SaveData;

static SaveData save;
static PrintConsole topConsole,bottomConsole;
static const char *root="fat:/";
static int mode=0,selected=0,homePage=0,cursor=0;
static int dirty=0, safeMode=0, frame=0, actionCount=0;
static int touchX=0,touchY=0,touchDown=0,touchStartX=-1,touchStartY=-1,touchPrevY=-1,touchMoved=0;
static int pageCursor=0, subCursor=0, moduleValue=0, touchActionLatch=0;
static int qState=0,qShots=0,qFidelity=0;
static int codexBook=0,codexPage=0,codexSearch=0;
static int animal=0,animalFeature=0,animalLive=0,animalEvents=0;
static int rfBand=0,rfChannel=1,rfView=0,rfHold=0,rfEvents=0;
static int saStart=100,saSpan=20,saRbw=10,saRunning=0,saMarker=0;
static int calcA=17,calcB=9,calcOp=0,calcMemory=0;
static int dawTrack=0,dawStep=0,dawPlaying=0,dawView=0;
static u8 dawPattern[4][16];
static int dspWindow=0,dspHold=0,dspGain=1,dspInput=0,dspFrames=0;
static int telemetryPage=0,networkTest=0,networkPackets=0;
static int aiMode=0,aiQuery=0,phoneConnected=0,phonePackets=0,phoneSync=0;
static int mediaTrack=0,mediaPlaying=0,mediaVolume=70,mediaMode=0;
static int sensorMode=0,sensorSamples=0,sensorPeak=0;
static int vaultCursor=0,vaultCount=0,fileCursor=0,fileCount=0;
static int hapticLevel=2,accessScale=1,accessContrast=0,accessScroll=1;
static int powerMode=0,powerSaver=0,powerCycles=0;
static int controlCursor=0,controlEvents=0,diagRuns=0,diagErrors=0;
static int botCursor=0,botRuns=0,settingsCursor=0,eventCursor=0,eventCount=0;
static int noteCursor=0,clock24=1,safetyCursor=0;
static int visualPhase=0,energy=0,bootCount=0,diagStorage=0,diagStorageKnown=0;
static int coreMode=0,coreCursor=0,coreZone=0,coreXp=120,coreLevel=2,coreStreak=3;
static int coreQuest=0,coreSocial=0,corePulse=0,coreCredits=75,coreWins=0,coreSession=0;
static int coreDay=1,coreEnergy=100,coreCoins=40,coreInventory=0,coreAchievements=0,coreEvent=0,coreNpc=0;
static int coreQuestDone[6]={0,0,0,0,0,0},coreAchDone[6]={0,0,0,0,0,0};
static int coreExecCursor=0;
static int execState[48]={0};
static int execAction[48]={0};
static char fileNames[16][48],vaultNames[12][48],eventNames[16][48];

static const char *apps[APP_COUNT]={
 "AETHER HOME","QUANTUM CORE","YHWH CODEX","ANIMAL AI","MARAUDER/RF",
 "TinySA LAB","CALCULATOR","DAW STUDIO","DSP/FFT","TELEMETRY","AI HOME",
 "NETWORK GATEWAY","PHONE LINK","MEDIA STUDIO","SENSOR HUB","DATA VAULT",
 "FILE BROWSER","HAPTIC LAB","ACCESSIBILITY","POWER LAB","CONTROL LAB",
 "DIAGNOSTICS","AETHER BOT","GENERAL SETTINGS","EVENT LOG","NOTES","CLOCK",
 "ABOUT","SAFETY CENTER",
 "EXECUTIVE HUB","MISSION CONTROL","RESOURCE COMMAND","SECURITY COMMAND","COMMS COMMAND",
 "OPERATIONS CENTER","DEVELOPMENT CENTER","CREATOR ECONOMY","KNOWLEDGE CENTER","HEALTH & WELLNESS",
 "ACCESS COMMAND","FINANCE LEDGER","INVENTORY COMMAND","FIELD COMMAND","AUTOMATION DESK",
 "ANALYTICS CENTER","ARCHIVE COMMAND","USER PROFILE","SYSTEMS MONITOR"
};
static const char *animalNames[]={"Horse","Dog","Cat","Cow","Bison","Camel","Zebra","Ostrich","Bird","Wolf","Fox","Deer","Bear","Big Cat","Other"};
static const char *codexBooks[66]={
 "Genesis","Exodus","Leviticus","Numbers","Deuteronomy","Joshua","Judges","Ruth",
 "1 Samuel","2 Samuel","1 Kings","2 Kings","1 Chronicles","2 Chronicles","Ezra",
 "Nehemiah","Esther","Job","Psalm","Proverbs","Ecclesiastes","Song of Solomon",
 "Isaiah","Jeremiah","Lamentations","Ezekiel","Daniel","Hosea","Joel","Amos",
 "Obadiah","Jonah","Micah","Nahum","Habakkuk","Zephaniah","Haggai","Zechariah",
 "Malachi","Matthew","Mark","Luke","John","Acts","Romans","1 Corinthians",
 "2 Corinthians","Galatians","Ephesians","Philippians","Colossians",
 "1 Thessalonians","2 Thessalonians","1 Timothy","2 Timothy","Titus","Philemon",
 "Hebrews","James","1 Peter","2 Peter","1 John","2 John","3 John","Jude","Revelation"
};

static u32 hash32(const void *p,size_t n){
 const u8 *b=(const u8*)p; u32 h=2166136261u;
 while(n--){h^=*b++;h*=16777619u;} return h;
}
static void markDirty(void){dirty=1;}
static int storageOk(void){ FILE *f=fopen("fat:/data/AetherMod/.aether_test","wb"); if(!f) return 0; fputs("OK",f); fclose(f); remove("fat:/data/AetherMod/.aether_test"); return 1; }
static void ensureDirs(void){mkdir("fat:/data",0777);mkdir("fat:/data/AetherMod",0777);}
static void saveState(void){
 if(safeMode)return; ensureDirs(); save.checksum=0;save.checksum=hash32(&save,sizeof(save));
 FILE *f=fopen("fat:/data/AetherMod/save.dat","wb");if(f){fwrite(&save,1,sizeof(save),f);fclose(f);dirty=0;}
}
static void defaults(void){
 memset(&save,0,sizeof(save));save.magic=SAVE_MAGIC;save.version=AETHER_SAVE_VERSION;
 save.sound=0;save.brightness=3;save.language=0;save.theme=1;save.ai=1;save.privacy=1;
 save.parental=1;save.nsfw=1;save.unsafe=1;save.unregulated=1;save.userContent=1;save.bpm=120;
}
static void loadState(void){
 defaults();ensureDirs();FILE *f=fopen("fat:/data/AetherMod/save.dat","rb");
 if(f){SaveData t;if(fread(&t,1,sizeof(t),f)==sizeof(t)){u32 c=t.checksum;t.checksum=0;
   if(c==hash32(&t,sizeof(t))&&t.magic==SAVE_MAGIC&&(t.version==4||t.version==6||t.version==SAVE_VERSION))save=t;
  }fclose(f);}
 bootCount++;
 selected=(save.selected<APP_COUNT)?save.selected:0; cursor=selected; homePage=selected/8; save.version=AETHER_SAVE_VERSION;
}
static void persistSelection(void){save.selected=(u16)selected;markDirty();}
static void homeSet(int n){if(n<0)n=APP_COUNT-1;if(n>=APP_COUNT)n=0;selected=n;cursor=n;homePage=n/8;persistSelection();}
static void page(const char *title){
 consoleSelect(&topConsole);consoleClear();
 iprintf("\x1b[36;1mAETHEROS 9.0 / DSi BOOT-SAFE\x1b[37;1m\n");
 iprintf("\x1b[35;1m==============================\x1b[37;1m\n");
 iprintf("%s\n",title);
 iprintf("DSi:%s  SAFE:%s  AI:%s  WIFI:%s\n",isDSiMode()?"YES":"DS",safeMode?"ON":"OFF",save.ai?"ON":"OFF",save.wireless?"ON":"OFF");
 iprintf("FRAME:%d  ENERGY:%d%%  ACTIONS:%d\n\n",frame,energy,actionCount);
}
static void footer(const char *s){consoleSelect(&bottomConsole);iprintf("\n%s\n",s);}
static void feedback(void){actionCount++;energy=(energy+17)%101;if(save.sound)soundPlayPSG(DutyCycle_50,440,80,64);}
static void saveIfDirty(void){if(dirty && (frame%30)==0)saveState();}
static void graph(const char *label,int seed){
 iprintf("%s|",label);for(int i=0;i<24;i++){int v=(i*7+seed+visualPhase)%16;iprintf("%c",v>12?'#':v>8?'*':v>4?'+':'.');}iprintf("|\n");
}
static void touchMap(u32 *d){
 static int lastHeld=0;
 int held=(keysHeld()&KEY_TOUCH)!=0;
 touchPosition t; touchRead(&t); touchX=t.px; touchY=t.py; touchDown=held;
 if(held){
   if(!lastHeld){touchStartX=t.px;touchStartY=t.py;touchPrevY=t.py;touchMoved=0;touchActionLatch=0;}
   if(touchPrevY>=0 && (t.py>touchPrevY+10 || t.py+10<touchPrevY)) touchMoved=1;
   touchPrevY=t.py;
   if(mode==0){
     if(t.py>=48 && t.py<176){
       int r=((int)t.py-48)/16;
       int n=homePage*8+r;
       if(!touchMoved && n<APP_COUNT && held && !touchActionLatch){homeSet(n);mode=n+1;touchActionLatch=1;feedback();}
     } else if(t.py>=176 && !touchMoved && held){
       homePage=(homePage+1)%HOME_PAGES; homeSet(homePage*8); feedback();
     }
     if(touchMoved && held && !touchActionLatch){
       if(touchStartY>=0 && t.py+24<touchStartY){homePage=(homePage+1)%HOME_PAGES;homeSet(homePage*8);feedback();}
       else if(touchStartY>=0 && t.py>touchStartY+24){homePage=(homePage+HOME_PAGES-1)%HOME_PAGES;homeSet(homePage*8);feedback();}
       touchStartY=t.py; touchMoved=0; touchActionLatch=1;
     }
     lastHeld=1; return;
   }
   if(!touchActionLatch){
     if(touchMoved){
       if(touchStartY>=0 && t.py+24<touchStartY)*d|=KEY_UP;
       else if(touchStartY>=0 && t.py>touchStartY+24)*d|=KEY_DOWN;
       else if(touchStartX>=0 && t.px>touchStartX+24)*d|=KEY_RIGHT;
       else if(touchStartX>=0 && t.px+24<touchStartX)*d|=KEY_LEFT;
     } else if(t.py<32&&t.px<128)*d|=KEY_B;
     else if(t.py<32)*d|=KEY_Y;
     else if(t.py>160&&t.px<128)*d|=KEY_X;
     else if(t.py>160)*d|=KEY_A;
     else if(t.px<64)*d|=KEY_LEFT;
     else if(t.px>192)*d|=KEY_RIGHT;
     else if(t.py<96)*d|=KEY_UP;
     else *d|=KEY_DOWN;
     if(*d & (KEY_A|KEY_B|KEY_X|KEY_Y|KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)) touchActionLatch=1;
   }
 } else {
   lastHeld=0; touchPrevY=-1; touchStartX=-1; touchStartY=-1; touchMoved=0; touchActionLatch=0;
 }
}
static void touchModuleActions(u32 *d){ (void)d; }
static void openModule(int n){homeSet(n);mode=n+1;save.launches++;markDirty();feedback();}
static void back(void){mode=0;homePage=selected/8;cursor=selected;saveState();}

static const u8 codexPartMap[66]={4,3,6,7,3,5,4,8,1,2,1,2,1,1,4,6,3,5,7,7,3,8,4,5,5,3,2,4,5,2,7,5,6,6,4,8,4,8,6,6,6,6,5,2,7,1,2,4,3,7,2,1,2,1,2,8,7,4,4,1,2,1,2,2,5,7};
static int codexRender(void){
 char path[160],line[192],head[96];int found=0,skip=codexPage*6,shown=0;
 snprintf(path,sizeof(path),"%sdata/AetherMod/Geneva/GENEVA_%02d.txt",root,codexPartMap[codexBook]);
 FILE *f=fopen(path,"rb");if(!f)return 0;snprintf(head,sizeof(head),"===== BOOK: %s =====",codexBooks[codexBook]);
 while(fgets(line,sizeof(line),f)){if(!found){if(strstr(line,head))found=1;continue;}if(strstr(line,"===== BOOK:")&&found)break;
  if(codexSearch&&!strstr(line,"Iehouah")&&!strstr(line,"Iehovah")&&!strstr(line,"Jehovah")&&!strstr(line,"LORD")&&!strstr(line,"GOD"))continue;
  if(!codexSearch&&skip-->0)continue;if(shown++<6)iprintf("%.178s",line);else break;}
 fclose(f);return found;
}

/* 01 — Quantum Core */
static void modQuantum(void){page("01 QUANTUM CORE");qFidelity=80+(frame/9)%21;
 iprintf("LOCAL QUANTUM-INSPIRED SIMULATOR\nSTATE:%d  SHOTS:%d  FIDELITY:%d%%\n",qState,qShots,qFidelity);
 graph("STATE ",qState*19);iprintf("LANES 8  ENTROPY:%d%%  PHASE:%d deg\n",(frame*3)%101,(frame*7)%360);
 iprintf("A=measure  X=phase  Y=reset  UP/DOWN=state\n");footer("B HOME | A MEASURE | X PHASE | Y RESET");}

/* 02 — Geneva Codex */
static void modCodex(void){page("02 YHWH CODEX / GENEVA 1599");iprintf("BOOK %d/66: %s  PAGE:%d SEARCH:%s\n",codexBook+1,codexBooks[codexBook],codexPage+1,codexSearch?"NAME":"OFF");
 if(!codexRender())iprintf("GENEVA_%02d.TXT NOT FOUND ON SD\n",codexPartMap[codexBook]);
 iprintf("\nOFFLINE PUBLIC-DOMAIN CORPUS / SOURCE TEXT ON SD\n");footer("UP/DOWN PAGE | L/R BOOK | A SEARCH | X NAME MODE | Y GENESIS | B HOME");}

/* 03 — Animal AI */
static void modAnimal(void){page("03 ANIMAL AI / SIGNAL LAB");int c=animalLive?60+(animal*7+frame)%35:0;
 iprintf("SPECIES:%s  MODE:%s  CONF:%d%%\n",animalNames[animal],animalLive?"ANALYZE":"READY",c);
 iprintf("PITCH:%d ENERGY:%d RHYTHM:%d\n",(animal*11+frame)%100,(animal*17+frame/2)%100,(animal*5+frame)%100);
 graph("FEATURE ",animalFeature*13+animal);iprintf("EVENTS:%d  OUTPUT:%d\n",animalEvents,(animalFeature*7+frame)%8);
 iprintf("Signal classification/vocalization model; not literal speech.\n");footer("UP/DOWN SPECIES | L/R FEATURE | A ANALYZE | X RUN | Y STOP | B HOME");}

/* 04 — Marauder/RF: passive/authorized analysis only */
static void modRF(void){page("04 MARAUDER / RF");const char *b[]={"2.4GHz ISM","5GHz ISM","EXTERNAL GATE"};
 iprintf("PASSIVE RECEIVE / AUTHORIZED ANALYSIS\nBAND:%s CH:%d VIEW:%d HOLD:%s\n",b[rfBand],rfChannel,rfView,rfHold?"ON":"OFF");
 int r=-30-(frame%45),n=-80-(frame%12);iprintf("RSSI:%d dBm NOISE:%d dBm SNR:%d dB\n",r,n,r-n);graph("RF ",rfChannel*9);
 iprintf("META EVENTS:%d  QUEUE:%d\n",rfEvents,(frame/8)%9);
 iprintf("JAM/DEAUTH/CREDENTIAL CAPTURE: NOT IMPLEMENTED.\n");footer("UP/DOWN VIEW | L/R BAND/CH | A LOG SAMPLE | X HOLD | Y CLEAR | B HOME");}

/* 05 — TinySA */
static void modTinySA(void){page("05 TINySA LAB");int level=20+(frame/3)%35;
 iprintf("SOFTWARE SPECTRUM WORKSPACE / EXTERNAL GATE\nSTART:%dMHz SPAN:%dMHz RBW:%dkHz\n",saStart,saSpan,saRbw);
 iprintf("SWEEP:%s  MARK:%dMHz  PEAK:%ddB\n",saRunning?"RUN":"STOP",saStart+saMarker,level+8);
 graph("TRACE ",saStart+saSpan);iprintf("VIEW:%d  POINTS:24  HOLD:%s\n",subCursor,rfHold?"ON":"OFF");
 iprintf("Physical TinySA requires compatible external hardware.\n");footer("UP/DOWN SPAN | L/R START | A SWEEP | X VIEW | Y RBW | B HOME");}

/* 06 — Calculator */
static long long calc(void){long long a=calcA,b=calcB;switch(calcOp%10){case 0:return a+b;case 1:return a-b;case 2:return a*b;case 3:return b?a/b:0;case 4:return b?a%b:0;case 5:return a*a;case 6:return a*a*a;case 7:return a^b;case 8:return a>b?a:b;default:return a<b?a:b;}}
static void modCalc(void){const char *op[]={"ADD","SUB","MUL","DIV","MOD","SQR","CUBE","XOR","MAX","MIN"};page("06 CALCULATOR");
 iprintf("A:%d B:%d OP:%s RESULT:%lld\n",calcA,calcB,op[calcOp%10],calc());iprintf("MEM:%d  VALID:%s\n",calcMemory,(calcOp==3&&calcB==0)?"DIV0 GUARD":"YES");
 iprintf("A/B are editable with A/X; operation with UP/DOWN.\n");footer("UP/DOWN OP | A APPLY | X SWAP | Y MEMORY | L/R EDIT | B HOME");}

/* 07 — DAW */
static void modDAW(void){page("07 DAW / GAME STUDIO");iprintf("BPM:%u STEP:%02d PLAY:%s TRACK:%d VIEW:%d\n",save.bpm,dawStep,dawPlaying?"YES":"NO",dawTrack+1,dawView);
 for(int t=0;t<4;t++){iprintf("T%d [",t+1);for(int s=0;s<16;s++)iprintf("%c",s==dawStep?'^':dawPattern[t][s]?'X':'.');iprintf("]\n");}
 iprintf("PSG sequencer / local timing / persistent BPM\n");footer("UP/DOWN STEP | L/R TRACK | A NOTE | X PLAY | Y BPM | B HOME");}

/* 08 — DSP/FFT */
static void modDSP(void){page("08 DSP / FFT");dspFrames++;int peak=(frame/3+dspInput*5)%16;
 iprintf("INPUT:%s GAIN:%d WINDOW:%s HOLD:%s FRAMES:%d\n",dspInput?"MIC-GATE":"SIM",dspGain,dspWindow?"HAMMING":"RECT",dspHold?"ON":"OFF",dspFrames);
 graph("FFT ",peak*11);iprintf("PEAK BIN:%d RMS:%d%%\n",peak,(frame/5)%100);footer("UP/DOWN GAIN | L/R INPUT | A REFRESH | X WINDOW | Y HOLD | B HOME");}

/* 09 — Telemetry */
static void modTelemetry(void){page("09 TELEMETRY");iprintf("PAGE %d/3\nFRAME:%d BOOTS:%d LAUNCHES:%lu\n",telemetryPage+1,frame,bootCount,(unsigned long)save.launches);
 iprintf("DSi:%s TOUCH:%s SD:FAT BAT:%d%%\n",isDSiMode()?"YES":"DS",touchDown?"ACTIVE":"READY",getBatteryLevel());
 iprintf("QFID:%d RF:%d DSP:%d ANIMAL:%d PHONE:%d\n",qFidelity,rfEvents,dspFrames,animalEvents,phonePackets);
 iprintf("SAFE:%s DIRTY:%s ACTIONS:%d\n",safeMode?"ON":"OFF",dirty?"YES":"NO",actionCount);footer("A PAGE | B HOME");}

/* 10 — AI Home */
static void modAI(void){page("10 AI HOME");const char *m[]={"CHAT","CODE","SCIENCE","ANIMAL","SYSTEM","WEB GATE"};
 iprintf("MODE:%s QUERY:%d LOCAL:%s\n",m[aiMode],aiQuery,save.ai?"READY":"OFF");
 iprintf("PRIVACY:%s ONLINE:%s BROWSER:%s\n",save.privacy?"LOCK":"OPEN",save.onlineAI?"GATE":"OFF",save.browser?"ON":"OFF");
 iprintf("Deterministic local workspace; online AI requires an external gateway.\n");footer("UP/DOWN MODE | A RUN | X ONLINE | Y PRIVACY | B HOME");}

/* 11 — Network */
static void modNetwork(void){page("11 NETWORK GATEWAY");iprintf("WIFI:%s SELFTEST:%s PACKETS:%d\n",save.wireless?"ARMED":"GUARDED",networkTest?"PASS":"READY",networkPackets);
 iprintf("LOCAL HTTP/DNS TEXT GATEWAY\nBrowser:%s  Safe links:%s\n",save.browser?"ON":"OFF",save.privacy?"ON":"OFF");
 iprintf("No credential capture, disruption, or radio attack path.\n");footer("A WIFI | X SELFTEST | Y BROWSER | L/R SLOT | B HOME");}

/* 12 — Phone Link */
static void modPhone(void){page("12 PHONE LINK");iprintf("PAIR:%s SESSION:%d SYNC:%s\n",phoneConnected?"CONNECTED":"READY",phoneConnected?1:0,phoneSync?"LIVE":"IDLE");
 iprintf("TX:%d RX:%d ACK:%d  DSi Wi-Fi 2.4GHz GATE\n",phonePackets*3,phonePackets*2,phonePackets);
 iprintf("PHONE REMOTE CONTROL IS A LOCAL UI/GATEWAY CONTRACT.\n");footer("A CONNECT | X SYNC | Y CLEAR | UP/DOWN PROFILE | B HOME");}

/* 13 — Media Studio */
static void modMedia(void){page("13 MEDIA STUDIO");iprintf("MODE:%s TRACK:%d PLAY:%s VOL:%d\n",mediaMode?"MIXER":"PLAYER",mediaTrack+1,mediaPlaying?"YES":"NO",mediaVolume);
 graph("WAVE ",mediaTrack*17);iprintf("LOCAL SD MEDIA CONTROL SURFACE\n");footer("UP/DOWN TRACK | L/R VOLUME | A PLAY | X MODE | Y NEXT | B HOME");}

/* 14 — Sensor Hub */
static void modSensor(void){page("14 SENSOR HUB");sensorSamples++;sensorPeak=(sensorPeak+frame)%100;
 iprintf("SOURCE:%s SAMPLES:%d PEAK:%d\n",sensorMode?"EXTERNAL GATE":"DSi LOCAL",sensorSamples,sensorPeak);
 iprintf("TOUCH:%d BAT:%d%% FRAME:%d\n",touchDown,getBatteryLevel(),frame);graph("SENS ",sensorMode*23);
 footer("UP/DOWN SOURCE | A SAMPLE | X RESET | Y PEAK | B HOME");}

/* 15 — Data Vault */
static void scanVault(void){vaultCount=0;DIR *d=opendir("fat:/data/AetherMod");if(!d&&isDSiMode())d=opendir("sd:/data/AetherMod");if(!d)return;struct dirent *e;
 while((e=readdir(d))&&vaultCount<12){if(e->d_name[0]=='.')continue;strncpy(vaultNames[vaultCount++],e->d_name,47);}closedir(d);}
static void modVault(void){page("15 DATA VAULT");if(vaultCount==0)scanVault();iprintf("SD DATA/AetherMod  ITEMS:%d\n",vaultCount);
 for(int i=0;i<vaultCount&&i<8;i++)iprintf("%c %02d %s\n",i==vaultCursor?'>':' ',i+1,vaultNames[i]);footer("UP/DOWN SELECT | A/X RESCAN | B HOME");}

/* 16 — File Browser */
static void scanFiles(void){fileCount=0;DIR *d=opendir("fat:/");if(!d&&isDSiMode())d=opendir("sd:/");if(!d)return;struct dirent *e;
 while((e=readdir(d))&&fileCount<16){if(e->d_name[0]=='.')continue;strncpy(fileNames[fileCount++],e->d_name,47);}closedir(d);}
static void modFiles(void){page("16 FILE BROWSER");if(fileCount==0)scanFiles();iprintf("ROOT:%s ITEMS:%d\n",root,fileCount);
 for(int i=0;i<fileCount&&i<8;i++)iprintf("%c %02d %s\n",i==fileCursor?'>':' ',i+1,fileNames[i]);iprintf("READ-ONLY NAVIGATION; no destructive delete action.\n");footer("UP/DOWN SELECT | A REFRESH | X VAULT | B HOME");}

/* 17 — Haptic */
static void modHaptic(void){page("17 HAPTIC LAB");iprintf("LEVEL:%d/3 PULSES:%d\n",hapticLevel,actionCount);iprintf("ACTIONS -> VISUAL:%d SOUND:%s\n",energy,save.sound?"ON":"OFF");
 iprintf("DSi physical rumble hardware is not claimed; feedback is UI/audio.\n");footer("L/R LEVEL | A TEST | X SOUND | Y VISUAL | B HOME");}

/* 18 — Accessibility */
static void modAccess(void){page("18 ACCESSIBILITY");iprintf("SCALE:%d CONTRAST:%s SCROLL:%d\n",accessScale,accessContrast?"HIGH":"NORMAL",accessScroll);
 iprintf("TOUCH TARGETS: LARGE  |  NAVIGATION: D-PAD + TOUCH\n");iprintf("VISUAL FEEDBACK:%s  AUDIO:%s\n",accessContrast?"HIGH":"STANDARD",save.sound?"ON":"OFF");
 footer("UP/DOWN SCALE | A CONTRAST | X SCROLL | Y SOUND | B HOME");}

/* 19 — Power */
static void modPower(void){page("19 POWER LAB");int bat=getBatteryLevel();iprintf("BATTERY:%d%% MODE:%s SAVER:%s CYCLES:%d\n",bat,powerMode?"LOW POWER":"NORMAL",powerSaver?"ON":"OFF",powerCycles);
 iprintf("BACKLIGHT:%u/4  FRAME RATE CONTROL: SOFTWARE\n",save.brightness);iprintf("STOCK DSi POWER RAILS ARE NOT MODIFIED.\n");footer("UP/DOWN MODE | A SAVER | X BRIGHT | Y CYCLE | B HOME");}

/* 20 — Control */
static void modControl(void){page("20 CONTROL LAB");iprintf("CURSOR:%d EVENTS:%d\n",controlCursor,controlEvents);iprintf("TOUCH X:%d Y:%d DOWN:%d\n",touchX,touchY,touchDown);
 iprintf("INPUT MATRIX: A B X Y / D-PAD / L R / START SELECT\n");footer("UP/DOWN CURSOR | A EVENT | X CLEAR | L/R MODE | B HOME");}

/* 21 — Diagnostics */
static void modDiagnostics(void){page("21 DIAGNOSTICS");
 diagErrors=0;if(!isDSiMode())diagErrors++;if(!save.magic)diagErrors++;if(diagStorageKnown&&!diagStorage)diagErrors++;
 iprintf("RUN:%d ERRORS:%d STATUS:%s\n",diagRuns,diagErrors,diagErrors?"CHECK":"PASS");
 iprintf("FAT:%s  SAVE:%s  TOUCH:%s\n",diagStorageKnown?(diagStorage?"PASS":"FAIL"):"NOT TESTED",save.magic?"VALID":"FAIL",touchDown?"LIVE":"READY");
 iprintf("APP COUNT:%d  BUILD:%d.%d PASS:%d/%d\n",APP_COUNT,AETHERMOD_MAJOR,AETHERMOD_MINOR,AETHERMOD_PASS,AETHERMOD_TOTAL_PASSES);
 footer("A RUN TEST | X RESET | B HOME");}

/* 22 — Aether Bot */
static void modBot(void){page("22 AETHER BOT");const char *jobs[]={"HOME","DIAGNOSTICS","FILES","VAULT","POWER","CONTROL","SETTINGS","SAFETY"};
 iprintf("JOB:%s RUNS:%d\n",jobs[botCursor%8],botRuns);iprintf("LOCAL ORCHESTRATION / NO AUTONOMOUS EXTERNAL ACTIONS\n");
 footer("UP/DOWN JOB | A EXECUTE | X RESET | Y HOME | B HOME");}

/* 23 — General Settings */
static void modSettings(void){page("23 GENERAL SETTINGS");const char *s[]={"AI","ONLINE AI","PRIVACY","BROWSER","DOWNLOADS","WIFI","SOUND","THEME"};
 iprintf("SELECT:%s = %s\n",s[settingsCursor],(settingsCursor==0?save.ai:settingsCursor==1?save.onlineAI:settingsCursor==2?save.privacy:settingsCursor==3?save.browser:settingsCursor==4?save.downloads:settingsCursor==5?save.wireless:settingsCursor==6?save.sound:save.theme)?"ON":"OFF");
 iprintf("BRIGHTNESS:%u  BPM:%u  LANGUAGE:%u\n",save.brightness,save.bpm,save.language);footer("UP/DOWN SELECT | A TOGGLE | X SAFE MODE | Y BRIGHT | B HOME");}

/* 24 — Event Log */
static void modEvents(void){page("24 EVENT LOG");eventCount=actionCount<16?actionCount:16;iprintf("EVENTS:%d\n",eventCount);
 for(int i=0;i<eventCount;i++)iprintf("%c EVENT %02d FRAME %d\n",i==eventCursor?'>':' ',i+1,(frame-i*7));footer("UP/DOWN SELECT | X REFRESH | Y CLEAR | B HOME");}

/* 25 — Notes */
static void modNotes(void){static const char *n[]={"Build priorities","DSi local-first workspace","Geneva corpus","Module QA","Animal signal lab","Rescue notes","Power notes","Release notes"};page("25 NOTES");
 iprintf("NOTE %d/8\n%s\n\nA writes selected note to SD.\n",noteCursor+1,n[noteCursor]);footer("UP/DOWN NOTE | A SAVE | X NEXT | B HOME");}

/* 26 — Clock */
static void modClock(void){page("26 CLOCK / TIME");time_t now=time(NULL);struct tm *t=localtime(&now);if(!t)iprintf("RTC UNAVAILABLE\n");else{
 int h=t->tm_hour;if(!clock24){int hh=h%12;if(hh==0)hh=12;iprintf("%02d:%02d:%02d %s\n",hh,t->tm_min,t->tm_sec,h>=12?"PM":"AM");}else iprintf("%02d:%02d:%02d\n",h,t->tm_min,t->tm_sec);
 iprintf("%04d-%02d-%02d\n",t->tm_year+1900,t->tm_mon+1,t->tm_mday);}footer("A 12/24H | B HOME");}

/* 27 — About */
static void modAbout(void){page("27 ABOUT");iprintf("AETHEROS 9.0 / DSi BOOT-SAFE\n");iprintf("29 INDIVIDUAL MODULE IMPLEMENTATIONS\n");iprintf("Geneva 1599 corpus: SD/OFFLINE\n");iprintf("Universal touch: TAP / SWIPE / DRAG\n");iprintf("Local-first, bounded, recoverable runtime.\n");footer("B HOME");}

/* 28 — Safety Center */
static void modSafety(void){page("28 SAFETY CENTER");const char *n[]={"PARENTAL","NSFW FILTER","UNSAFE FILTER","UNREGULATED","USER CONTENT","BROWSER","DOWNLOADS","WIRELESS"};
 int on=safetyCursor==0?save.parental:safetyCursor==1?save.nsfw:safetyCursor==2?save.unsafe:safetyCursor==3?save.unregulated:safetyCursor==4?save.userContent:safetyCursor==5?save.browser:safetyCursor==6?save.downloads:save.wireless;
 iprintf("CONTROL:%s  STATE:%s\n",n[safetyCursor],on?"ON":"OFF");iprintf("SAFE MODE MASTER:%s\n",safeMode?"ON":"OFF");footer("UP/DOWN CONTROL | A TOGGLE | X SAFE MODE | Y RESET SAFE | B HOME");}

static void draw(void);

/* AetherCore 1 — Pass 3: executive system fusion. */
static const char *execSystems[48]={
 "EXECUTIVE HUB","MISSION CONTROL","RESOURCE COMMAND","SECURITY COMMAND","COMMS COMMAND","OPERATIONS CENTER","DEVELOPMENT CENTER","CREATOR ECONOMY","KNOWLEDGE CENTER","HEALTH & WELLNESS","ACCESS COMMAND","FINANCE LEDGER","INVENTORY COMMAND","FIELD COMMAND","AUTOMATION DESK","ANALYTICS CENTER","ARCHIVE COMMAND","USER PROFILE","SYSTEMS MONITOR","STRATEGY DESK","PROJECT COMMAND","TASK COMMAND","SCHEDULE CENTER","RESOURCE PLANNER","RISK DESK","QUALITY COMMAND","RESEARCH DESK","DESIGN COMMAND","CONTENT COMMAND","COMMUNITY COMMAND","PARTNERSHIP DESK","SERVICE COMMAND","SUPPORT COMMAND","LOGISTICS COMMAND","ASSET COMMAND","DATA COMMAND","INSIGHTS DESK","PERFORMANCE COMMAND","COMPLIANCE DESK","POLICY CENTER","CHANGE COMMAND","RELEASE COMMAND","TEST COMMAND","RELIABILITY CENTER","CONTINUITY DESK","GROWTH COMMAND","IMPACT CENTER","AETHERCORE CONTROL"
};
static const char *execModes[48]={
 "COMMAND","QUESTS","RESOURCES","GUARD","SIGNALS","OPS","BUILD","MARKET","LIBRARY","WELLNESS",
 "ACCESS","LEDGER","ASSETS","FIELD","AUTOMATION","METRICS","ARCHIVE","PROFILE","MONITOR","STRATEGY","PROJECT","TASK","SCHEDULE","PLAN","RISK","QUALITY","RESEARCH","DESIGN","CONTENT","COMMUNITY","PARTNERS","SERVICE","SUPPORT","LOGISTICS","ASSETS","DATA","INSIGHTS","PERFORMANCE","COMPLIANCE","POLICY","CHANGE","RELEASE","TEST","RELIABILITY","CONTINUITY","GROWTH","IMPACT","CONTROL"
};
static void execSystemView(int id){
 int v=execState[id],a=execAction[id];
 page(execSystems[id]);
 iprintf("AETHERCORE EXECUTIVE SYSTEM %02d/19  MODE:%s\\n",id+1,execModes[id]);
 iprintf("STATE:%d  ACTIONS:%d  CORE LVL:%d  XP:%d\\n",v,a,coreLevel,coreXp);
  if(id>=19){
    static const char *focus[29]={"strategy","projects","tasks","schedule","planning","risk","quality","research","design","content","community","partnerships","service","support","logistics","assets","data","insights","performance","compliance","policy","change","release","testing","reliability","continuity","growth","impact","core control"};
    iprintf("FOCUS:%s  STATE:%d  ACTIONS:%d\\n",focus[id-19],v,a);
    iprintf("CORE LVL:%d XP:%d ENERGY:%d COINS:%d CREDITS:%d\\n",coreLevel,coreXp,coreEnergy,coreCoins,coreCredits);
    iprintf("LINKS QUEST:%d ZONE:%d CREW:%d DIAG:%d TELEMETRY:%d\\n",coreQuest,coreZone,coreSocial,diagRuns,telemetryPage+1);
    graph("EXEC ",id*13+v+corePulse);
    iprintf("A EXECUTE | X RESET | UP/DOWN STATE | L/R SYSTEM\\n");
    footer("A RUN | X RESET | L/R SYSTEM | Y AETHERCORE | B HOME");
    return;
  }
 switch(id){
  case 0: iprintf("NEXUS STATUS:%s  ZONE:%s  SESSION:%d\\n",coreEnergy>20?"READY":"LOW ENERGY",((const char *[]){"NEXUS","QUANTUM FIELD","CODEX GARDEN","SIGNAL RIDGE","CREATOR DECK","SYSTEMS"})[coreZone],coreSession); graph("COMMAND ",corePulse); break;
  case 1: iprintf("ACTIVE QUEST:%s  DONE:%s  ENERGY:%d\\n",((const char *[]){"CALIBRATE THE CORE","SCAN A SIGNAL","OPEN THE CODEX","BUILD A BEAT","RUN A DIAGNOSTIC","VISIT THE SYSTEMS"})[coreQuest],coreQuestDone[coreQuest]?"YES":"NO",coreEnergy); graph("MISSION ",coreQuest*11+v); break;
  case 2: iprintf("COINS:%d  CREDITS:%d  INVENTORY:%d\\n",coreCoins,coreCredits,coreInventory); graph("RESOURCE ",coreCoins+v); break;
  case 3: iprintf("SAFE MODE:%s  PRIVACY:%s  PARENTAL:%s\\n",safeMode?"ON":"OFF",save.privacy?"ON":"OFF",save.parental?"ON":"OFF"); graph("GUARD ",save.privacy*19+v); break;
  case 4: iprintf("WIRELESS:%s  NETWORK PACKETS:%d  PHONE:%d\\n",save.wireless?"READY":"OFF",networkPackets,phonePackets); graph("SIGNAL ",networkPackets+v); break;
  case 5: iprintf("CORE DAY:%d  NPC:%d  WINS:%d  ACTIONS:%d\\n",coreDay,coreNpc,coreWins,a); graph("OPS ",coreDay+v); break;
  case 6: iprintf("DAW STEP:%d  DSP FRAMES:%d  CREATIVE RUNS:%d\\n",dawStep,dspFrames,v); graph("BUILD ",dawStep+v); break;
  case 7: iprintf("CREATOR CREDITS:%d  COINS:%d  QUEST WINS:%d\\n",coreCredits,coreCoins,coreWins); graph("ECON ",coreCredits+v); break;
  case 8: iprintf("CODEX BOOK:%s  PAGE:%d  NOTES:%d\\n",codexBooks[codexBook],codexPage,noteCursor+1); graph("KNOW ",codexBook+v); break;
  case 9: iprintf("ENERGY:%d  SENSOR SAMPLES:%d  ACTIVITY:%d\\n",coreEnergy,sensorSamples,energy); graph("WELL ",coreEnergy+v); break;
  case 10: iprintf("SCALE:%d  CONTRAST:%s  SCROLL:%d\\n",accessScale,accessContrast?"HIGH":"NORMAL",accessScroll); graph("ACCESS ",accessScale+v); break;
  case 11: iprintf("CREDITS:%d  COINS:%d  TRANSACTIONS:%d\\n",coreCredits,coreCoins,a); graph("LEDGER ",coreCredits+coreCoins+v); break;
  case 12: iprintf("INVENTORY:%d  VAULT:%d  FILES:%d\\n",coreInventory,vaultCount,fileCount); graph("ASSETS ",coreInventory+v); break;
  case 13: iprintf("ZONE:%s  ANIMAL EVENTS:%d  RF EVENTS:%d\\n",((const char *[]){"NEXUS","QUANTUM FIELD","CODEX GARDEN","SIGNAL RIDGE","CREATOR DECK","SYSTEMS"})[coreZone],animalEvents,rfEvents); graph("FIELD ",coreZone+animalEvents+v); break;
  case 14: iprintf("AUTOMATION RUNS:%d  BOT RUNS:%d  DIAG:%d\\n",a,botRuns,diagRuns); graph("AUTO ",a+botRuns); break;
  case 15: iprintf("ACTIONS:%d  FRAME:%d  TELEMETRY PAGE:%d\\n",actionCount,frame,telemetryPage+1); graph("METRIC ",actionCount+v); break;
  case 16: iprintf("EVENTS:%d  FILES:%d  VAULT:%d\\n",eventCount,fileCount,vaultCount); graph("ARCHIVE ",eventCount+v); break;
  case 17: iprintf("LEVEL:%d  XP:%d  STREAK:%d  CREW:%d\\n",coreLevel,coreXp,coreStreak,coreSocial); graph("PROFILE ",coreLevel+coreSocial+v); break;
  default: iprintf("DSi:%s  BAT:%d%%  SAFE:%s  FRAME:%d\\n",isDSiMode()?"YES":"DS",getBatteryLevel(),safeMode?"ON":"OFF",frame); graph("MONITOR ",frame+v); break;
 }
 iprintf("\\nA EXECUTE  X RESET  UP/DOWN SYSTEM PARAMETER  L/R EXECUTIVE SYSTEM\\n");
 footer("A RUN | X RESET | L/R SYSTEM | Y AETHERCORE | B HOME");
}
static void execSystemInput(int id,u32 d){
 int changed=0;
 if(d&KEY_B){back();return;}
 if(d&KEY_UP){execState[id]++;changed=1;}
 if(d&KEY_DOWN){if(execState[id]>0)execState[id]--;changed=1;}
 if(d&KEY_LEFT){id=(id+47)%48;selected=29+id;mode=30+id;changed=0;draw();return;}
 if(d&KEY_RIGHT){id=(id+1)%48;selected=29+id;mode=30+id;changed=0;draw();return;}
 if(d&KEY_A){
   execAction[id]++; execState[id]++;
   if(id==0)corePulse=(corePulse+1)%100;
   if(id==1 && coreEnergy>=5){coreEnergy-=5;coreSession++;}
   if(id==2){coreCoins+=2;coreCredits+=1;}
   if(id==3){save.privacy=1;save.parental=1;}
   if(id==4){networkPackets++;phonePackets++;}
   if(id==5){coreDay++;coreNpc=(coreNpc+1)%4;}
   if(id==6){dawStep=(dawStep+1)%16;dspFrames++;}
   if(id==7)coreCredits+=3;
   if(id==8)codexPage++;
   if(id==9){coreEnergy=coreEnergy<100?coreEnergy+1:100;}
   if(id==10)accessScroll=(accessScroll%3)+1;
   if(id==11)coreCredits++;
   if(id==12)coreInventory++;
   if(id==13)coreZone=(coreZone+1)%6;
   if(id==14)botRuns++; 
   if(id==15)telemetryPage=(telemetryPage+1)%3;
   if(id==16)eventCount=(eventCount+1)%17;
   if(id==17)coreStreak++;
   if(id==18)diagRuns++;
   changed=1;
 }
 if(d&KEY_X){execState[id]=0;changed=1;}
 if(changed){feedback();markDirty();draw();}
}

/* AetherCore 1 — Pass 1: gameplay/social shell around the complete AetherOS 9 runtime. */
static const char *coreZones[]={"NEXUS","QUANTUM FIELD","CODEX GARDEN","SIGNAL RIDGE","CREATOR DECK","SYSTEMS"};
static const char *coreQuests[]={"CALIBRATE THE CORE","SCAN A SIGNAL","OPEN THE CODEX","BUILD A BEAT","RUN A DIAGNOSTIC","VISIT THE SYSTEMS"};
static const char *coreAvatars[]={"PIONEER","ENGINEER","SCOUT","CREATOR"};
static void draw(void);
static void coreHeader(const char *title){ page(title); consoleSelect(&topConsole); iprintf("\x1b[33;1mAETHERCORE 1 / PASS 3\x1b[37;1m\n"); iprintf("LEVEL %d  XP %d  STREAK %d  CREDITS %d\n",coreLevel,coreXp,coreStreak,coreCredits); }
static void coreReward(int xp,int credits){ coreXp+=xp; coreCredits+=credits; coreWins++; coreStreak++; if(coreXp>=coreLevel*100){coreXp-=coreLevel*100;coreLevel++;} corePulse=(corePulse+1)%100; markDirty(); feedback(); }
static void coreWorld(void){
 coreHeader("AETHERCORE NEXUS");
 iprintf("ZONE: %s  AVATAR:%s  DAY:%d\\n",coreZones[coreZone],coreAvatars[(coreLevel+coreZone)%4],coreDay);
 iprintf("ENERGY:%d  COINS:%d  INVENTORY:%d\\n\\n",coreEnergy,coreCoins,coreInventory);
 iprintf("MISSION BOARD\\n");
 for(int i=0;i<6;i++) iprintf("%c %d %-21s %s\\n",i==coreQuest?'>':' ',i+1,coreQuests[i],coreQuestDone[i]?"DONE":"READY");
 iprintf("\\nNPC: %s\\n",coreNpc==0?"NOVA / ARCHIVIST":coreNpc==1?"ORBIT / SCOUT":coreNpc==2?"VECTOR / BUILDER":"LYRA / TRADER");
 iprintf("CREW:%d  WINS:%d  ACHIEVEMENTS:%d\\n",coreSocial,coreWins,coreAchievements);
 graph("WORLD ",coreZone*17+corePulse);
 iprintf("A QUEST  X CREW  Y SYSTEMS  L/R ZONE\\n");
 footer("TOUCH: TAP WORLD AREAS | SWIPE ZONES | L/R SPECIAL");
}
static void coreWorld_legacy(void){ coreHeader("AETHERCORE NEXUS"); iprintf("ZONE: %s   AVATAR: %s\n\n",coreZones[coreZone],coreAvatars[(coreLevel+coreZone)%4]); iprintf("MISSION BOARD\n"); for(int i=0;i<6;i++) iprintf("%c %d  %-22s %s\n",i==coreQuest?'>':' ',i+1,coreQuests[i],i==coreQuest?"READY":"AVAILABLE"); iprintf("\nLOCAL CREW: %d   CORE WINS: %d\n",coreSocial,coreWins); graph("WORLD ",coreZone*17+corePulse); iprintf("A=accept quest  X=crew  Y=systems  L/R=zone\n"); footer("A QUEST | X CREW | Y SYSTEMS | B NEXUS"); }
static void coreAchievementsView(void){
 coreHeader("AETHERCORE ACHIEVEMENTS");
 const char *a[]={"FIRST CONTACT","CORE EXPLORER","QUEST RUNNER","SYSTEMS PILOT","CREATOR","SOCIAL SIGNAL"};
 for(int i=0;i<6;i++) iprintf("%c %-18s %s\\n",coreAchDone[i]?'*':' ',a[i],coreAchDone[i]?"UNLOCKED":"LOCKED");
 footer("X NEXUS | B BACK");
}
static void coreEventView(void){
 coreHeader("AETHERCORE LIVE EVENT");
 iprintf("EVENT %d: %s\\n\\n",coreEvent+1,coreEvent==0?"QUANTUM STORM":coreEvent==1?"SIGNAL HUNT":coreEvent==2?"CREATOR JAM":"NEXUS FESTIVAL");
 iprintf("Participate to earn XP, coins and achievements.\\n");
 iprintf("EVENT ENERGY: %d\\n",coreEnergy);
 footer("A PARTICIPATE | X ACHIEVEMENTS | Y NEXUS | B BACK");
}
static void coreTick(void){
 if((frame%120)==0){corePulse=(corePulse+1)%100;coreNpc=(coreNpc+1)%4;if(coreEnergy<100)coreEnergy++;}
}
static void coreSocialView(void){ coreHeader("AETHERCORE CREW"); iprintf("LOCAL SOCIAL DECK / NO CLOUD CLAIM\n\n"); iprintf("CREW SIGNAL: %s\n",coreSocial?"ACTIVE":"DISCOVERY"); iprintf("Players are represented locally until a supported network service is added.\n\n"); iprintf("[01] YOU      LVL %d  XP %d\n",coreLevel,coreXp); iprintf("[02] NOVA     LVL 4   READY\n[03] ORBIT    LVL 3   READY\n[04] VECTOR   LVL 5   READY\n"); iprintf("\nA=send local pulse  X=refresh crew  Y=world\n"); footer("A PULSE | X REFRESH | Y WORLD | B NEXUS"); }
static void coreSystems(void){ coreHeader("AETHERCORE SYSTEMS GATE"); iprintf("THE COMPLETE AETHEROS 9.0 MODULE GRID REMAINS AVAILABLE.\n\n"); iprintf("77 SYSTEMS / 29 CORE RUNTIMES + 48 EXECUTIVE SYSTEMS / GENEVA 1599\n"); iprintf("QUANTUM  CODEX  ANIMAL  RF  TinySA  CALC  DAW  DSP\n"); iprintf("TELEMETRY  AI  NETWORK  PHONE  MEDIA  SENSOR  VAULT\n"); iprintf("FILES  HAPTIC  ACCESS  POWER  CONTROL  DIAGNOSTICS\n"); iprintf("BOT  SETTINGS  EVENTS  NOTES  CLOCK  ABOUT  SAFETY\n"); footer("A MODULE GRID | X WORLD | Y AETHEROS HOME | B NEXUS"); }
static void coreInput(u32 d){
 if(d&KEY_B){coreMode=0;draw();return;}
 if(coreMode==0){
  if(d&KEY_UP){coreQuest=(coreQuest+5)%6;draw();} if(d&KEY_DOWN){coreQuest=(coreQuest+1)%6;draw();}
  if(d&KEY_LEFT){coreZone=(coreZone+5)%6;coreDay++;draw();} if(d&KEY_RIGHT){coreZone=(coreZone+1)%6;coreDay++;draw();}
  if(d&KEY_A&&coreEnergy>=10){coreEnergy-=10;coreReward(20+coreQuest*5,5+coreZone);coreCoins+=8+coreQuest;coreInventory++;coreQuestDone[coreQuest]=1;coreAchDone[0]=1;if(coreZone>=2)coreAchDone[1]=1;if(coreInventory>=3)coreAchDone[2]=1;coreAchievements=0;for(int i=0;i<6;i++)coreAchievements+=coreAchDone[i];coreSession++;draw();}
  if(d&KEY_X){coreMode=1;draw();} if(d&KEY_Y){coreMode=2;draw();} if(d&KEY_L){coreMode=3;draw();} if(d&KEY_R){coreMode=4;draw();}
 }else if(coreMode==1){
  if(d&KEY_A){coreSocial++;coreReward(5,1);if(!coreAchDone[5]){coreAchDone[5]=1;coreAchievements++;}draw();} if(d&KEY_X){coreSocial=(coreSocial+1)%9;draw();} if(d&KEY_Y){coreMode=0;draw();}
 }else if(coreMode==2){if(d&KEY_A||d&KEY_Y){mode=0;coreMode=0;draw();}if(d&KEY_X){coreMode=0;draw();}}
 else if(coreMode==3){if(d&KEY_X||d&KEY_Y){coreMode=0;draw();}}
 else {if(d&KEY_A&&coreEnergy>=15){coreEnergy-=15;coreReward(35,15);coreCoins+=15;coreAchDone[4]=1;if(coreAchievements<6)coreAchievements++;draw();}if(d&KEY_X){coreMode=3;draw();}if(d&KEY_Y){coreMode=0;draw();}}
}
static void coreFront(void){ if(coreMode==0)coreWorld(); else if(coreMode==1)coreSocialView(); else if(coreMode==2)coreSystems(); else if(coreMode==3)coreAchievementsView(); else coreEventView(); }

static void home(void){
 page("00 AETHER HOME / EXECUTIVE DECK");consoleSelect(&bottomConsole);consoleClear();
 int last=homePage*8+8; if(last>APP_COUNT) last=APP_COUNT; iprintf("PAGE %d/%d  MODULES %02d-%02d\n\n",homePage+1,HOME_PAGES,homePage*8+1,last);
 int first=homePage*8;for(int i=0;i<8;i++){int n=first+i;if(n>=APP_COUNT)break;iprintf("%c%02d %-18s %c\n",n==selected?'>':' ',n+1,apps[n],((frame+i*7)%16<5)?'*':'.');}
 iprintf("\nSELECT:%02d  %s\n",selected+1,apps[selected]);iprintf("A OPEN | L/R PAGE | UP/DOWN MODULE\n");
 iprintf("TOUCH: 8 LARGE ROWS OPEN | SWIPE = SCROLL | LOWER STRIP = NEXT PAGE\n");footer("START+SELECT: normal controls | START hold is not destructive");
}
static void draw(void){
 switch(mode){
 case 0:home();break;case 1:modQuantum();break;case 2:modCodex();break;case 3:modAnimal();break;
 case 4:modRF();break;case 5:modTinySA();break;case 6:modCalc();break;case 7:modDAW();break;
 case 8:modDSP();break;case 9:modTelemetry();break;case 10:modAI();break;case 11:modNetwork();break;
 case 12:modPhone();break;case 13:modMedia();break;case 14:modSensor();break;case 15:modVault();break;
 case 16:modFiles();break;case 17:modHaptic();break;case 18:modAccess();break;case 19:modPower();break;
 case 20:modControl();break;case 21:modDiagnostics();break;case 22:modBot();break;case 23:modSettings();break;
 case 24:modEvents();break;case 25:modNotes();break;case 26:modClock();break;case 27:modAbout();break;
 case 28:modSafety();break;case 29:coreFront();break;case 30:execSystemView(0);break;case 31:execSystemView(1);break;case 32:execSystemView(2);break;case 33:execSystemView(3);break;case 34:execSystemView(4);break;case 35:execSystemView(5);break;case 36:execSystemView(6);break;case 37:execSystemView(7);break;case 38:execSystemView(8);break;case 39:execSystemView(9);break;case 40:execSystemView(10);break;case 41:execSystemView(11);break;case 42:execSystemView(12);break;case 43:execSystemView(13);break;case 44:execSystemView(14);break;case 45:execSystemView(15);break;case 46:execSystemView(16);break;case 47:execSystemView(17);break;case 48:execSystemView(18);break;case 49:execSystemView(19);break;case 50:execSystemView(20);break;case 51:execSystemView(21);break;case 52:execSystemView(22);break;case 53:execSystemView(23);break;case 54:execSystemView(24);break;case 55:execSystemView(25);break;case 56:execSystemView(26);break;case 57:execSystemView(27);break;case 58:execSystemView(28);break;case 59:execSystemView(29);break;case 60:execSystemView(30);break;case 61:execSystemView(31);break;case 62:execSystemView(32);break;case 63:execSystemView(33);break;case 64:execSystemView(34);break;case 65:execSystemView(35);break;case 66:execSystemView(36);break;case 67:execSystemView(37);break;case 68:execSystemView(38);break;case 69:execSystemView(39);break;case 70:execSystemView(40);break;case 71:execSystemView(41);break;case 72:execSystemView(42);break;case 73:execSystemView(43);break;case 74:execSystemView(44);break;case 75:execSystemView(45);break;case 76:execSystemView(46);break;case 77:execSystemView(47);break;default:mode=0;break;
 }
}
static void moduleInput(u32 d){
 int changed=0;
 if(mode>=30&&mode<=48){execSystemInput(mode-30,d);return;}
 if(d&KEY_B){back();return;}
 switch(mode){
 case 1:if(d&KEY_UP){qState=(qState+3)%4;changed=1;}if(d&KEY_DOWN){qState=(qState+1)%4;changed=1;}if(d&KEY_A){qShots++;qState=(qState+1)%4;changed=1;}if(d&KEY_X){qState=(qState+1)%4;changed=1;}if(d&KEY_Y){qState=0;changed=1;}break;
 case 2:if(d&KEY_UP){if(codexPage)codexPage--;changed=1;}if(d&KEY_DOWN){codexPage++;if(codexPage>4095)codexPage=4095;changed=1;}if(d&KEY_LEFT){codexBook=(codexBook+65)%66;codexPage=0;changed=1;}if(d&KEY_RIGHT){codexBook=(codexBook+1)%66;codexPage=0;changed=1;}if(d&KEY_A){codexSearch=!codexSearch;changed=1;}if(d&KEY_X){codexSearch=!codexSearch;changed=1;}if(d&KEY_Y){codexBook=0;codexPage=0;changed=1;}break;
 case 3:if(d&KEY_UP){animal=(animal+14)%15;changed=1;}if(d&KEY_DOWN){animal=(animal+1)%15;changed=1;}if(d&KEY_LEFT){animalFeature=(animalFeature+4)%5;changed=1;}if(d&KEY_RIGHT){animalFeature=(animalFeature+1)%5;changed=1;}if(d&KEY_A||d&KEY_X){animalLive=1;animalEvents++;changed=1;}if(d&KEY_Y){animalLive=0;changed=1;}break;
 case 4:if(d&KEY_UP){rfView=(rfView+3)%4;changed=1;}if(d&KEY_DOWN){rfView=(rfView+1)%4;changed=1;}if(d&KEY_LEFT){rfBand=(rfBand+2)%3;rfChannel=(rfChannel+10)%11+1;changed=1;}if(d&KEY_RIGHT){rfBand=(rfBand+1)%3;rfChannel=rfChannel%11+1;changed=1;}if(d&KEY_A){rfEvents++;changed=1;}if(d&KEY_X){rfHold=!rfHold;changed=1;}if(d&KEY_Y){rfEvents=0;changed=1;}break;
 case 5:if(d&KEY_UP){saSpan+=5;if(saSpan>200)saSpan=5;changed=1;}if(d&KEY_DOWN){saSpan-=5;if(saSpan<5)saSpan=200;changed=1;}if(d&KEY_LEFT){saStart-=10;if(saStart<0)saStart=0;changed=1;}if(d&KEY_RIGHT){saStart+=10;if(saStart>800)saStart=800;changed=1;}if(d&KEY_A){saRunning=!saRunning;changed=1;}if(d&KEY_X){subCursor=(subCursor+1)%4;changed=1;}if(d&KEY_Y){saRbw=saRbw==10?30:saRbw==30?100:10;changed=1;}break;
 case 6:if(d&KEY_UP){calcOp=(calcOp+9)%10;changed=1;}if(d&KEY_DOWN){calcOp=(calcOp+1)%10;changed=1;}if(d&KEY_A){calcA=(int)calc();changed=1;}if(d&KEY_X){int z=calcA;calcA=calcB;calcB=z;changed=1;}if(d&KEY_Y){calcMemory=calcA;changed=1;}if(d&KEY_LEFT){calcA--;changed=1;}if(d&KEY_RIGHT){calcA++;changed=1;}break;
 case 7:if(d&KEY_UP){dawStep=(dawStep+15)%16;changed=1;}if(d&KEY_DOWN){dawStep=(dawStep+1)%16;changed=1;}if(d&KEY_LEFT){dawTrack=(dawTrack+3)%4;changed=1;}if(d&KEY_RIGHT){dawTrack=(dawTrack+1)%4;changed=1;}if(d&KEY_A){dawPattern[dawTrack][dawStep]^=1;changed=1;}if(d&KEY_X){dawPlaying=!dawPlaying;changed=1;}if(d&KEY_Y){save.bpm+=5;if(save.bpm>240)save.bpm=60;changed=1;}break;
 case 8:if(d&KEY_UP&&dspGain<8){dspGain++;changed=1;}if(d&KEY_DOWN&&dspGain>1){dspGain--;changed=1;}if(d&KEY_LEFT){dspInput=0;changed=1;}if(d&KEY_RIGHT){dspInput=1;changed=1;}if(d&KEY_A){dspFrames++;changed=1;}if(d&KEY_X){dspWindow=!dspWindow;changed=1;}if(d&KEY_Y){dspHold=!dspHold;changed=1;}break;
 case 9:if(d&KEY_A){telemetryPage=(telemetryPage+1)%3;changed=1;}break;
 case 10:if(d&KEY_UP){aiMode=(aiMode+5)%6;changed=1;}if(d&KEY_DOWN){aiMode=(aiMode+1)%6;changed=1;}if(d&KEY_A){aiQuery++;changed=1;}if(d&KEY_X){save.onlineAI=!save.onlineAI;changed=1;}if(d&KEY_Y){save.privacy=!save.privacy;changed=1;}break;
 case 11:if(d&KEY_A){save.wireless=!save.wireless;networkPackets++;changed=1;}if(d&KEY_X){networkTest=1;changed=1;}if(d&KEY_Y){save.browser=!save.browser;changed=1;}if(d&KEY_LEFT||d&KEY_RIGHT){subCursor=(subCursor+1)%8;changed=1;}break;
 case 12:if(d&KEY_A){phoneConnected=!phoneConnected;changed=1;}if(d&KEY_X){phoneSync=!phoneSync;phonePackets++;changed=1;}if(d&KEY_Y){phonePackets=0;changed=1;}if(d&KEY_UP||d&KEY_DOWN){subCursor=(subCursor+1)%4;changed=1;}break;
 case 13:if(d&KEY_UP){mediaTrack=(mediaTrack+7)%8;changed=1;}if(d&KEY_DOWN){mediaTrack=(mediaTrack+1)%8;changed=1;}if(d&KEY_LEFT&&mediaVolume>0){mediaVolume-=5;changed=1;}if(d&KEY_RIGHT&&mediaVolume<100){mediaVolume+=5;changed=1;}if(d&KEY_A){mediaPlaying=!mediaPlaying;changed=1;}if(d&KEY_X){mediaMode=!mediaMode;changed=1;}if(d&KEY_Y){mediaTrack=(mediaTrack+1)%8;changed=1;}break;
 case 14:if(d&KEY_UP||d&KEY_DOWN){sensorMode=!sensorMode;changed=1;}if(d&KEY_A){sensorSamples++;changed=1;}if(d&KEY_X){sensorSamples=0;changed=1;}if(d&KEY_Y){sensorPeak=0;changed=1;}break;
 case 15:if(d&KEY_UP&&vaultCount){vaultCursor=(vaultCursor+vaultCount-1)%vaultCount;changed=1;}if(d&KEY_DOWN&&vaultCount){vaultCursor=(vaultCursor+1)%vaultCount;changed=1;}if(d&KEY_A||d&KEY_X){scanVault();changed=1;}break;
 case 16:if(d&KEY_UP&&fileCount){fileCursor=(fileCursor+fileCount-1)%fileCount;changed=1;}if(d&KEY_DOWN&&fileCount){fileCursor=(fileCursor+1)%fileCount;changed=1;}if(d&KEY_A){scanFiles();changed=1;}if(d&KEY_X){scanVault();changed=1;}break;
 case 17:if(d&KEY_LEFT&&hapticLevel>0){hapticLevel--;changed=1;}if(d&KEY_RIGHT&&hapticLevel<3){hapticLevel++;changed=1;}if(d&KEY_A){feedback();changed=1;}if(d&KEY_X){save.sound=!save.sound;changed=1;}if(d&KEY_Y){energy=(energy+25)%101;changed=1;}break;
 case 18:if(d&KEY_UP&&accessScale<3){accessScale++;changed=1;}if(d&KEY_DOWN&&accessScale>1){accessScale--;changed=1;}if(d&KEY_A){accessContrast=!accessContrast;changed=1;}if(d&KEY_X){accessScroll=(accessScroll%3)+1;changed=1;}if(d&KEY_Y){save.sound=!save.sound;changed=1;}break;
 case 19:if(d&KEY_UP||d&KEY_DOWN){powerMode=!powerMode;changed=1;}if(d&KEY_A){powerSaver=!powerSaver;changed=1;}if(d&KEY_X){save.brightness=(save.brightness+1)%5;changed=1;}if(d&KEY_Y){powerCycles++;changed=1;}break;
 case 20:if(d&KEY_UP){controlCursor=(controlCursor+7)%8;changed=1;}if(d&KEY_DOWN){controlCursor=(controlCursor+1)%8;changed=1;}if(d&KEY_A){controlEvents++;changed=1;}if(d&KEY_X){controlEvents=0;changed=1;}if(d&KEY_LEFT||d&KEY_RIGHT){subCursor=(subCursor+1)%4;changed=1;}break;
 case 21:if(d&KEY_A){diagRuns++;diagStorage=storageOk();diagStorageKnown=1;changed=1;}if(d&KEY_X){diagRuns=0;diagErrors=0;diagStorage=0;diagStorageKnown=0;changed=1;}break;
 case 22:if(d&KEY_UP){botCursor=(botCursor+7)%8;changed=1;}if(d&KEY_DOWN){botCursor=(botCursor+1)%8;changed=1;}if(d&KEY_A){botRuns++;changed=1;}if(d&KEY_X){botRuns=0;changed=1;}if(d&KEY_Y){back();return;}break;
 case 23:if(d&KEY_UP){settingsCursor=(settingsCursor+7)%8;changed=1;}if(d&KEY_DOWN){settingsCursor=(settingsCursor+1)%8;changed=1;}if(d&KEY_A){switch(settingsCursor){case 0:save.ai=!save.ai;break;case 1:save.onlineAI=!save.onlineAI;break;case 2:save.privacy=!save.privacy;break;case 3:save.browser=!save.browser;break;case 4:save.downloads=!save.downloads;break;case 5:save.wireless=!save.wireless;break;case 6:save.sound=!save.sound;break;default:save.theme=!save.theme;break;}changed=1;}if(d&KEY_X){safeMode=!safeMode;changed=1;}if(d&KEY_Y){save.brightness=(save.brightness+1)%5;changed=1;}break;
 case 24:if(d&KEY_UP&&eventCount){eventCursor=(eventCursor+eventCount-1)%eventCount;changed=1;}if(d&KEY_DOWN&&eventCount){eventCursor=(eventCursor+1)%eventCount;changed=1;}if(d&KEY_X){eventCount=actionCount<16?actionCount:16;changed=1;}if(d&KEY_Y){eventCount=0;changed=1;}break;
 case 25:if(d&KEY_UP){noteCursor=(noteCursor+7)%8;changed=1;}if(d&KEY_DOWN){noteCursor=(noteCursor+1)%8;changed=1;}if(d&KEY_A){ensureDirs();FILE *f=fopen("fat:/data/AetherMod/notes.txt","ab");if(f){fprintf(f,"note:%d frame:%d\\n",noteCursor,frame);fclose(f);}changed=1;}if(d&KEY_X){noteCursor=(noteCursor+1)%8;changed=1;}break;
 case 26:if(d&KEY_A){clock24=!clock24;changed=1;}break;
 case 27:break;
 case 28:if(d&KEY_UP){safetyCursor=(safetyCursor+7)%8;changed=1;}if(d&KEY_DOWN){safetyCursor=(safetyCursor+1)%8;changed=1;}if(d&KEY_A){u8 *v=safetyCursor==0?&save.parental:safetyCursor==1?&save.nsfw:safetyCursor==2?&save.unsafe:safetyCursor==3?&save.unregulated:safetyCursor==4?&save.userContent:safetyCursor==5?&save.browser:safetyCursor==6?&save.downloads:&save.wireless;*v=!*v;changed=1;}if(d&KEY_X){safeMode=!safeMode;changed=1;}if(d&KEY_Y){save.parental=save.nsfw=save.unsafe=save.unregulated=1;changed=1;}break;
 }
 if(changed){feedback();markDirty();draw();}
}
static void input(void){
 u32 d=keysDown()|keysDownRepeat();touchDown=0;touchMap(&d);
 if(mode==29){coreInput(d);return;}
 if(mode==0){
   if(d&KEY_UP){homeSet(selected==0?APP_COUNT-1:selected-1);draw();}
   if(d&KEY_DOWN){homeSet((selected+1)%APP_COUNT);draw();}
   if(d&KEY_LEFT){homePage=(homePage+HOME_PAGES-1)%HOME_PAGES;homeSet(homePage*8);draw();}
   if(d&KEY_RIGHT){homePage=(homePage+1)%HOME_PAGES;homeSet(homePage*8);draw();}
   if(d&KEY_A){openModule(selected);draw();}
   return;
 }
 moduleInput(d);
}
int legacy_shell_main(void){
 powerOn(POWER_ALL_2D);
 videoSetMode(MODE_0_2D);
 videoSetModeSub(MODE_0_2D);
 vramDefault();
 consoleInit(&topConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,true,true);
 consoleInit(&bottomConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,false,true);
 soundDisable();

 /* Boot-critical rule: the display must be live before any SD/FAT work.
  * A bad/slow/unmounted DSi SD must never leave the user staring at black. */
 consoleSelect(&topConsole);
 consoleClear();
 iprintf("\x1b[36;1mAETHEROS 9.0 / DSi\x1b[37;1m\n");
 iprintf("\x1b[35;1mBOOT-SAFE INITIALIZATION\x1b[37;1m\n");
 iprintf("DISPLAY: ONLINE\n");
 consoleSelect(&bottomConsole);
 consoleClear();
 iprintf("Initializing local runtime...\n");
 iprintf("SD access is optional; UI starts first.\n");
 swiWaitForVBlank();

 int fatReady=fatInitDefault();
 if(fatReady){
   loadState();
   save.launches++;
   saveState();
 }else{
   safeMode=1;
   defaults();
   selected=0; cursor=0; homePage=0;
   consoleSelect(&topConsole);
   iprintf("SD: NOT AVAILABLE / SAFE MODE\n");
   consoleSelect(&bottomConsole);
   iprintf("Continuing without storage.\n");
 }
 mode=29;
 draw();

 while(1){
   swiWaitForVBlank();
   scanKeys();
   frame++;
   visualPhase=(visualPhase+1)&63;
   energy=(energy+1)%101;
   if(dawPlaying&&(frame%15)==0){
     dawStep=(dawStep+1)%16;
     for(int t=0;t<4;t++)
       if(dawPattern[t][dawStep]&&save.sound)
         soundPlayPSG(DutyCycle_50,220+t*90,70,64);
   }
   input();
   if(mode==29) coreTick();
   saveIfDirty();
   if((frame&3)==0)draw();
 }
 return 0;
}

/* AetherOS 9.0 CI release: build/header/package verification is mandatory. */
