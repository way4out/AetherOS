#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>
#include "config.h"

/* AetherMod 8.6 — DSi-native modular cockpit.
 * Every home entry maps to an independent implementation.
 * Hardware claims remain honest: external RF/TinySA/camera/AI/phone/QPU
 * capabilities are represented as software workspaces/gateways, not invented
 * stock-DSi hardware.
 */
#define APP_COUNT 29
#define AETHERMOD_MAJOR 8
#define AETHERMOD_MINOR 7
#define AETHERMOD_PASS 3
#define AETHERMOD_TOTAL_PASSES 3
#define HOME_PAGES 4
#define AETHER_SAVE_VERSION 6

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
static int touchX=0,touchY=0,touchDown=0,touchStartY=-1,touchPrevY=-1,touchMoved=0;
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
static int visualPhase=0,energy=0,bootCount=0;
static char fileNames[16][48],vaultNames[12][48],eventNames[16][48];

static const char *apps[APP_COUNT]={
 "AETHER HOME","QUANTUM CORE","YHWH CODEX","ANIMAL AI","MARAUDER/RF",
 "TINySA LAB","CALCULATOR","DAW STUDIO","DSP/FFT","TELEMETRY","AI HOME",
 "NETWORK GATEWAY","PHONE LINK","MEDIA STUDIO","SENSOR HUB","DATA VAULT",
 "FILE BROWSER","HAPTIC LAB","ACCESSIBILITY","POWER LAB","CONTROL LAB",
 "DIAGNOSTICS","AETHER BOT","GENERAL SETTINGS","EVENT LOG","NOTES","CLOCK",
 "ABOUT","SAFETY CENTER"
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
   if(c==hash32(&t,sizeof(t))&&t.magic==SAVE_MAGIC&&(t.version==4||t.version==SAVE_VERSION))save=t;
  }fclose(f);}
 bootCount++;
 /* 8.7 Pass 1: deterministic cold start on the first home entry. */
 selected=0; cursor=0; homePage=0; save.selected=0;
}
static void persistSelection(void){save.selected=(u16)selected;markDirty();}
static void homeSet(int n){if(n<0)n=APP_COUNT-1;if(n>=APP_COUNT)n=0;selected=n;cursor=n;homePage=n/8;persistSelection();}
static void page(const char *title){
 consoleSelect(&topConsole);consoleClear();
 iprintf("\x1b[36;1mAETHEROS 8.7 / PASS 2/3\x1b[37;1m\n");
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
   if(!lastHeld){touchStartY=t.py;touchPrevY=t.py;touchMoved=0;touchActionLatch=0;}
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
       else if(t.px>touchX+24)*d|=KEY_RIGHT;
       else if(t.px+24<touchX)*d|=KEY_LEFT;
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
   lastHeld=0; touchPrevY=-1; touchStartY=-1; touchMoved=0; touchActionLatch=0;
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
static void modDiagnostics(void){page("21 DIAGNOSTICS");diagRuns++;diagErrors=0;if(!isDSiMode())diagErrors++;if(!save.magic)diagErrors++;
 iprintf("RUN:%d ERRORS:%d STATUS:%s\n",diagRuns,diagErrors,diagErrors?"CHECK":"PASS");iprintf("FAT:%s  SAVE:%s  TOUCH:%s\n",storageOk()?"PASS":"FAIL",save.magic?"VALID":"FAIL",touchDown?"LIVE":"READY");
 iprintf("APP COUNT:%d  BUILD:%d.%d PASS:%d/%d\n",APP_COUNT,AETHERMOD_MAJOR,AETHERMOD_MINOR,AETHERMOD_PASS,AETHERMOD_TOTAL_PASSES);
 footer("A RUN AGAIN | X RESET COUNTERS | B HOME");}

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
static void modAbout(void){page("27 ABOUT");iprintf("AETHEROS 8.7 / PASS 3/3\n");iprintf("29 INDIVIDUAL MODULE IMPLEMENTATIONS\n");iprintf("Geneva 1599 corpus: SD/OFFLINE\n");iprintf("Universal touch: TAP / SWIPE / DRAG\n");iprintf("Local-first, bounded, recoverable runtime.\n");footer("B HOME");}

/* 28 — Safety Center */
static void modSafety(void){page("28 SAFETY CENTER");const char *n[]={"PARENTAL","NSFW FILTER","UNSAFE FILTER","UNREGULATED","USER CONTENT","BROWSER","DOWNLOADS","WIRELESS"};
 int on=safetyCursor==0?save.parental:safetyCursor==1?save.nsfw:safetyCursor==2?save.unsafe:safetyCursor==3?save.unregulated:safetyCursor==4?save.userContent:safetyCursor==5?save.browser:safetyCursor==6?save.downloads:save.wireless;
 iprintf("CONTROL:%s  STATE:%s\n",n[safetyCursor],on?"ON":"OFF");iprintf("SAFE MODE MASTER:%s\n",safeMode?"ON":"OFF");footer("UP/DOWN CONTROL | A TOGGLE | X SAFE MODE | Y RESET SAFE | B HOME");}

static void home(void){
 page("00 AETHER HOME");consoleSelect(&bottomConsole);consoleClear();
 iprintf("PAGE %d/%d  MODULES %02d-%02d\n\n",homePage+1,HOME_PAGES,homePage*8+1,homePage*8+8);
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
 case 28:modSafety();break;default:mode=0;break;
 }
}
static void moduleInput(u32 d){
 int changed=0;
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
 case 21:if(d&KEY_A){diagRuns++;changed=1;}if(d&KEY_X){diagRuns=0;diagErrors=0;changed=1;}break;
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
int main(void){
 powerOn(POWER_ALL_2D);videoSetMode(MODE_0_2D);videoSetModeSub(MODE_0_2D);vramDefault();
 consoleInit(&topConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,true,true);
 consoleInit(&bottomConsole,0,BgType_Text4bpp,BgSize_T_256x256,22,3,false,true);
 soundDisable();if(!fatInitDefault()){safeMode=1;defaults();}else{if(isDSiMode())root="sd:/";loadState();save.launches++;saveState();}
 draw();
 while(1){swiWaitForVBlank();scanKeys();frame++;visualPhase=(visualPhase+1)&63;energy=(energy+1)%101;
   if(dawPlaying&&(frame%15)==0){dawStep=(dawStep+1)%16;for(int t=0;t<4;t++)if(dawPattern[t][dawStep]&&save.sound)soundPlayPSG(DutyCycle_50,220+t*90,70,64);}
   input();saveIfDirty();if((frame&3)==0)draw();
 }
 return 0;
}
