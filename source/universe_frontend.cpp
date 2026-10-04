#include <nds.h>
#include <stdio.h>

extern "C" int legacy_shell_main(void);

static void universe_draw(int cursor){
  consoleSelect(NULL);
  printf("");
  (void)cursor;
}

/* Universe Simulator+ is a lightweight, deterministic DSi launcher.
 * It never replaces the hardware-safe AetherOS runtime; it selects the
 * operating role and then hands off to the full local runtime.
 */
extern "C" int universe_frontend(void){
  powerOn(POWER_ALL_2D);
  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);
  vramDefault();

  PrintConsole top, bottom;
  consoleInit(&top,0,BgType_Text4bpp,BgSize_T_256x256,22,3,true,true);
  consoleInit(&bottom,0,BgType_Text4bpp,BgSize_T_256x256,30,0,false,true);
  soundEnable();

  int cursor=0;
  const char *roles[]={
    "QUANTUM CORE","SYSTEMS OPERATIONS","CREATOR / LAB",
    "COMMS / GATEWAYS","RECOVERY / DIAGNOSTICS"
  };
  while(1){
    consoleSelect(&top); consoleClear();
    printf("\x1b[36;1mUNIVERSE SIMULATOR+\x1b[37;1m\n");
    printf("\x1b[35;1mAETHEROS DSi QUANTUM KIT\x1b[37;1m\n");
    printf("==============================\n");
    printf("LOCAL-FIRST / OFFLINE-CAPABLE\n");
    printf("ROLE: %s\n\n",roles[cursor]);
    printf("A  ENTER FULL AETHEROS RUNTIME\n");
    printf("UP/DOWN  SELECT ROLE\n");
    printf("TOUCH  TAP A ROLE ROW\n");
    printf("B  ENTER DEFAULT SYSTEMS\n");

    consoleSelect(&bottom); consoleClear();
    printf("UNIVERSE SIMULATOR+\n\n");
    for(int i=0;i<5;i++)
      printf("%c %d  %-24s\n",i==cursor?'>':' ',i+1,roles[i]);
    printf("\nA=LAUNCH   D-PAD=SELECT\n");
    printf("TOUCH ROWS = SELECT / LAUNCH\n");
    printf("All modules remain capability-gated\n");
    printf("to real DSi hardware or authorized gateways.\n");

    swiWaitForVBlank();
    scanKeys();
    u32 d=keysDown();
    touchPosition t; touchRead(&t);
    if((keysHeld()&KEY_TOUCH) && t.py>=24 && t.py<184){
      int r=((int)t.py-24)/32;
      if(r>=0 && r<5){ cursor=r; break; }
    }
    if(d&KEY_UP) cursor=(cursor+4)%5;
    if(d&KEY_DOWN) cursor=(cursor+1)%5;
    if(d&(KEY_A|KEY_B)) break;
  }
  return legacy_shell_main();
}
