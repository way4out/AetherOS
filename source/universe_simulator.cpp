#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { int x,y,vx,vy,mass,r,alive; u16 color; } Body;
static Body b[24];
static int paused=0, selected=1, speed=1, frameNo=0;
static int palette=0, toolbar=0, tool=0;
static u16 *fb=nullptr;
static bool renderReady=false;

static u16 color15(int r,int g,int bl){return RGB15((r>>3)&31,(g>>3)&31,(bl>>3)&31)|BIT(15);}
static int clampi(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static const char *toolNames[3][6]={
  {"SCAN","MOVE","MAP","TARGET","ZOOM","RESET"},
  {"BUILD","PAINT","LINK","LIFT","PLACE","RESET"},
  {"SAMPLE","FIELD","TRACE","MEASURE","MARK","RESET"}
};

static u16 suitColor(){
  static const u16 c[4]={0x7FFF,RGB15(20,30,31)|BIT(15),RGB15(5,28,12)|BIT(15),RGB15(28,8,25)|BIT(15)};
  return c[palette&3];
}

static void reset_sim(void){
  for(int i=0;i<24;i++) b[i].alive=0;
  b[0].x=128;b[0].y=96;b[0].vx=0;b[0].vy=0;b[0].mass=1200;b[0].r=8;b[0].alive=1;b[0].color=color15(255,210,60);
  for(int i=1;i<24;i++){
    int ring=18+(i*9)%65,side=(i&1)?1:-1;
    b[i].x=128+side*ring;b[i].y=96+(i*17)%31-15;
    b[i].vx=0;b[i].vy=side*(38+(i%5)*5);b[i].mass=4+i%5;b[i].r=2+i%3;b[i].alive=1;
    b[i].color=color15(50+(i*31)%190,80+(i*47)%150,150+(i*17)%100);
  }
  selected=1;frameNo=0;speed=1;tool=0;
}

static void clear_fb(u16 c){if(!fb)return;for(int i=0;i<256*192;i++)fb[i]=c;}
static void rect(int x,int y,int w,int h,u16 c){
  if(!fb)return;int x0=clampi(x,0,255),y0=clampi(y,0,191),x1=clampi(x+w,0,256),y1=clampi(y+h,0,192);
  for(int yy=y0;yy<y1;yy++)for(int xx=x0;xx<x1;xx++)fb[yy*256+xx]=c;
}
static void dot(int x,int y,int r,u16 c){
  if(!fb||x<0||x>=256||y<0||y>=192)return;
  for(int yy=-r;yy<=r;yy++)for(int xx=-r;xx<=r;xx++)if(xx*xx+yy*yy<=r*r){int X=x+xx,Y=y+yy;if(X>=0&&X<256&&Y>=0&&Y<192)fb[Y*256+X]=c;}
}
static void line(int x0,int y0,int x1,int y1,u16 c){
  if(!fb)return;int dx=x1-x0,dy=y1-y0,steps=abs(dx)>abs(dy)?abs(dx):abs(dy);if(!steps){dot(x0,y0,1,c);return;}
  for(int i=0;i<=steps;i++){int x=x0+dx*i/steps,y=y0+dy*i/steps;if(x>=0&&x<256&&y>=0&&y<192)fb[y*256+x]=c;}
}
static void starfield(){
  if(!fb)return;
  for(int i=0;i<90;i++){int x=(i*73+frameNo/3)%256,y=(i*47+i*i)%192;fb[y*256+x]=(i%7==0)?color15(120,170,255):color15(40,70,120);}
}

static void draw_first_person_world(){
  if(!fb)return;
  clear_fb(color15(2,5,18));
  rect(0,0,256,118,color15(3,8,28));
  rect(0,118,256,74,color15(12,12,18));
  for(int y=120;y<192;y+=12)line(0,y,255,y,color15(18,18,28));
  for(int x=-256;x<512;x+=32)line(128,118,x,192,color15(18,18,28));
  starfield();
  for(int i=0;i<24;i++)if(b[i].alive){
    if(i==0){dot(b[i].x,b[i].y,11,color15(255,130,20));dot(b[i].x,b[i].y,7,b[i].color);}
    else{dot(b[i].x,b[i].y,b[i].r,b[i].color);if(i==selected)dot(b[i].x,b[i].y,b[i].r+3,color15(255,255,255));}
  }
  u16 suit=suitColor();
  rect(83,169,28,23,suit);rect(145,169,28,23,suit);
  dot(90,168,9,color15(220,170,120));dot(166,168,9,color15(220,170,120));
  rect(112,161,32,8,color15(35,35,45));rect(120,153,16,10,color15(55,55,70));
  u16 toolCol=(toolbar==0)?color15(50,210,255):(toolbar==1?color15(90,255,120):color15(255,190,50));
  if(tool==0||tool==3){rect(125,132,6,30,toolCol);dot(128,130,5,toolCol);}
  else if(tool==1||tool==4){line(116,154,140,134,toolCol);rect(137,130,10,5,toolCol);}
  else{rect(122,140,12,22,toolCol);line(118,144,138,144,toolCol);}
  line(124,96,132,96,color15(255,255,255));line(128,92,128,100,color15(255,255,255));
}

static void status(PrintConsole &sub){
  consoleSelect(&sub);consoleClear();
  printf("\x1b[36;1mUNIVERSE SIMULATOR+\x1b[37;1m\n");
  printf("FIRST-PERSON DSi UNIVERSE\n\n");
  printf("USER: NEW LOCAL EXPLORER\n");
  printf("AVATAR PALETTE: %d/4\n",palette+1);
  printf("TOOL BASE: %s\n",toolbar==0?"EXPLORER":toolbar==1?"BUILDER":"SCIENCE");
  printf("TOOL: %s\n",toolNames[toolbar][tool]);
  printf("WORLD BODIES: 24  FRAME: %d\n",frameNo);
  printf("SPEED: %dx  %s\n\n",speed,paused?"PAUSED":"LIVE");
  printf("TOUCH: toolbar rows / tool slots\n");
  printf("A = tool action   X = pause\n");
  printf("Y = reset         L/R = palette\n");
  printf("D-PAD = move/select body\n");
  printf("B = cycle tool base\n");
  printf("\n[01] %s  [02] %s\n",toolNames[toolbar][0],toolNames[toolbar][1]);
  printf("[03] %s  [04] %s\n",toolNames[toolbar][2],toolNames[toolbar][3]);
  printf("[05] %s  [06] %s\n",toolNames[toolbar][4],toolNames[toolbar][5]);
}

static void simulate(){
  for(int step=0;step<speed;step++)for(int i=1;i<24;i++)if(b[i].alive){
    int ax=0,ay=0;
    for(int j=0;j<24;j++)if(i!=j&&b[j].alive){int dx=b[j].x-b[i].x,dy=b[j].y-b[i].y,d2=dx*dx+dy*dy+20,p=(b[j].mass*5)/d2;ax+=dx*p/8;ay+=dy*p/8;}
    b[i].vx=clampi(b[i].vx+ax,-70,70);b[i].vy=clampi(b[i].vy+ay,-70,70);
    b[i].x+=b[i].vx/24;b[i].y+=b[i].vy/24;
    if(b[i].x<4||b[i].x>251){b[i].vx=-b[i].vx;b[i].x=clampi(b[i].x,4,251);}
    if(b[i].y<4||b[i].y>187){b[i].vy=-b[i].vy;b[i].y=clampi(b[i].y,4,187);}
  }
  frameNo++;
}

static void selectToolByTouch(int px,int py){
  if(py<72){toolbar=(py/24)%3;tool=0;return;}
  int row=(py-72)/24,col=(px>=128)?1:0,idx=row*2+col;
  if(idx>=0&&idx<6)tool=idx;
}

extern "C" int universe_simulator_main(void){
  powerOn(POWER_ALL_2D);
  videoSetMode(MODE_5_2D);
  videoSetModeSub(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  int bg=bgInit(3,BgType_Bmp16,BgSize_B16_256x256,0,0);
  fb=(u16*)bgGetGfxPtr(bg);
  renderReady=(fb!=nullptr);
  PrintConsole sub;
  consoleInit(&sub,0,BgType_Text4bpp,BgSize_T_256x256,30,0,false,true);
  reset_sim();
  if(!renderReady){
    consoleSelect(&sub);consoleClear();
    printf("AETHEROS RECOVERY\nUNIVERSE SIMULATOR+\n\nBITMAP BUFFER UNAVAILABLE\nSAFE TEXT MODE ACTIVE\n\nA/B RETURN  Y RESET\n");
  }else{
    draw_first_person_world();
    status(sub);
  }
  swiWaitForVBlank();
  while(1){
    swiWaitForVBlank();
    scanKeys();
    u32 d=keysDown();
    touchPosition t;touchRead(&t);
    if(d&KEY_X){paused=!paused;status(sub);}
    if(d&KEY_Y){reset_sim();status(sub);}
    if(d&KEY_L){palette=(palette+3)%4;status(sub);}
    if(d&KEY_R){palette=(palette+1)%4;status(sub);}
    if(d&KEY_B){toolbar=(toolbar+1)%3;tool=0;status(sub);}
    if(d&KEY_LEFT){selected=(selected+21)%23+1;b[selected].vx-=8;}
    if(d&KEY_RIGHT){selected=selected%23+1;b[selected].vx+=8;}
    if(d&KEY_UP)b[selected].vy-=8;
    if(d&KEY_DOWN)b[selected].vy+=8;
    if(d&KEY_A){b[selected].vx+=b[selected].x<128?10:-10;b[selected].vy+=b[selected].y<96?8:-8;}
    if(d&KEY_TOUCH){selectToolByTouch((int)t.px,(int)t.py);status(sub);}
    if(!paused)simulate();
    if(renderReady)draw_first_person_world();
  }
  return 0;
}
