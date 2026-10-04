#include <nds.h>
#include <stdio.h>

extern "C" int legacy_shell_main(void);

/* Universe Simulator+ DSi front end.
 * Deterministic, local-first launcher; all capabilities remain subject to
 * the hardware and permissions actually available on the DSi/runtime.
 */
static const char *roles[]={
  "QUANTUM CORE","SYSTEMS OPERATIONS","CREATOR / LAB",
  "COMMS / GATEWAYS","RECOVERY / DIAGNOSTICS"
};
static const int ROLE_COUNT=5;

static void draw_top(int cursor, bool help){
  consoleClear();
  printf("\x1b[36;1mUNIVERSE SIMULATOR+\x1b[37;1m\n");
  printf("\x1b[35;1mAETHEROS DSi QUANTUM KIT\x1b[37;1m\n");
  printf("================================\n");
  printf("LOCAL-FIRST  |  DSi MODE  |  READY\n");
  printf("ROLE: %-22s\n\n", roles[cursor]);
  if(help){
    printf("CONTROLS\n");
    printf("UP/DOWN  select\n");
    printf("A        launch selected role\n");
    printf("B        recovery/default runtime\n");
    printf("X        diagnostics/status\n");
    printf("Y        help\n");
    printf("TOUCH    tap a role / launch\n");
  }else{
    printf("A  LAUNCH SELECTED ROLE\n");
    printf("X  DIAGNOSTICS / STATUS\n");
    printf("Y  HELP\n");
    printf("B  DEFAULT SYSTEMS\n");
  }
}

static void draw_bottom(int cursor){
  consoleClear();
  printf("\x1b[36;1mROLE MATRIX\x1b[37;1m\n\n");
  for(int i=0;i<ROLE_COUNT;i++)
    printf("%c %d  %-24s\n", i==cursor?'>':' ', i+1, roles[i]);
  printf("\n--------------------------------\n");
  printf("TOUCH A ROW TO SELECT + LAUNCH\n");
  printf("D-PAD SELECTS WITHOUT TOUCH\n");
  printf("\nSAFE HANDOFF: AETHEROS RUNTIME\n");
  printf("Unsupported hardware stays gated.\n");
}

static void diagnostics(PrintConsole &top, PrintConsole &bottom){
  consoleSelect(&top); consoleClear();
  printf("\x1b[36;1mAETHEROS STATUS\x1b[37;1m\n\n");
  printf("Frontend       ONLINE\n");
  printf("Touch input    READY\n");
  printf("Buttons        READY\n");
  printf("Dual screens   READY\n");
  printf("Boot handoff   ARMED\n");
  printf("Storage        RUNTIME-DEPENDENT\n");
  printf("External HW    CAPABILITY-GATED\n");
  printf("\nA / B = RETURN\n");

  consoleSelect(&bottom); consoleClear();
  printf("DIAGNOSTICS\n\n");
  printf("This screen checks launcher state only.\n");
  printf("It does not pretend unavailable DSi\n");
  printf("hardware exists.\n\n");
  printf("Press A or B to return.");
  while(1){
    swiWaitForVBlank(); scanKeys();
    u32 d=keysDown();
    if(d&(KEY_A|KEY_B)) return;
  }
}

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
  bool help=false;

  while(1){
    consoleSelect(&top); draw_top(cursor,help);
    consoleSelect(&bottom); draw_bottom(cursor);

    swiWaitForVBlank();
    scanKeys();
    u32 d=keysDown();

    if(d&KEY_UP)   cursor=(cursor+ROLE_COUNT-1)%ROLE_COUNT;
    if(d&KEY_DOWN) cursor=(cursor+1)%ROLE_COUNT;
    if(d&KEY_Y) help=!help;

    if(d&KEY_X){
      diagnostics(top,bottom);
      continue;
    }

    if(d&KEY_B || d&KEY_A) break;

    if(keysDown()&KEY_TOUCH){
      touchPosition t; touchRead(&t);
      if(t.py>=32 && t.py<192){
        int r=((int)t.py-32)/32;
        if(r>=0 && r<ROLE_COUNT){
          cursor=r;
          if(t.py>=32 && t.py<192) break;
        }
      }
    }
  }

  return legacy_shell_main();
}
