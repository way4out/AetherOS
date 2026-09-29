#include <nds.h>
#include <nds/camera.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "aether_hardware.h"
#define AETHER_CAMERA_NDMA 2
#define AETHER_PREVIEW_W 256
#define AETHER_PREVIEW_H 192
#define AETHER_FULL_W 640
#define AETHER_FULL_H 480
#define AETHER_MIC_BYTES 32768
static bool cameraReady=false;
static bool micReady=false;
static bool micRunning=false;
static int cameraDevice=CAMERA_OUTER;
static int previewCount=0;
static int captureCount=0;
static int micPeak=0;
static int micRms=0;
static u16 previewBuffer[AETHER_PREVIEW_W*AETHER_PREVIEW_H];
static u16 captureBuffer[AETHER_FULL_W*AETHER_FULL_H];
static u8 micBuffer[AETHER_MIC_BYTES];
static void micCallback(void *completedBuffer,int length){
 if(!completedBuffer||length<=0)return;
 const s16 *samples=(const s16*)memUncached(completedBuffer);
 int count=length/(int)sizeof(s16),peak=0;
 uint64_t sum=0;
 for(int i=0;i<count;i++){int v=samples[i],av=v<0?-v:v;if(av>peak)peak=av;sum+=(uint64_t)av*(uint64_t)av;}
 micPeak=peak;
 if(count>0)micRms=(int)sqrt32((u32)(sum/(uint64_t)count));
}
static void ensureCameraDir(void){mkdir("fat:/data",0777);mkdir("fat:/data/AetherMod",0777);mkdir("fat:/data/AetherMod/camera",0777);}
static void writePreviewPPM(const char *path){
 FILE *f=fopen(path,"wb");if(!f)return;
 fprintf(f,"P6\n%d %d\n255\n",AETHER_PREVIEW_W,AETHER_PREVIEW_H);
 for(int i=0;i<AETHER_PREVIEW_W*AETHER_PREVIEW_H;i++){u16 p=previewBuffer[i];u8 rgb[3];
  rgb[0]=(u8)(((p>>11)&31)*255/31);rgb[1]=(u8)(((p>>6)&31)*255/31);rgb[2]=(u8)(((p>>1)&31)*255/31);fwrite(rgb,1,3,f);}
 fclose(f);
}
void aetherHardwareInit(void){
 cameraReady=false;micReady=false;micRunning=false;
 if(!isDSiMode())return;
 if(cameraInit()){cameraReady=true;cameraDevice=CAMERA_OUTER;cameraSelect(CAMERA_OUTER);}
 soundEnable();micReady=true;
}
void aetherHardwareShutdown(void){
 if(micRunning){soundMicOff();micRunning=false;}
 if(cameraReady){cameraDeinit();cameraReady=false;}
}
bool aetherCameraAvailable(void){return cameraReady;}
bool aetherMicAvailable(void){return micReady;}
int aetherCameraDevice(void){return cameraDevice;}
void aetherCameraToggle(void){
 if(!cameraReady || cameraTransferActive() || ndmaBusy(AETHER_CAMERA_NDMA)) return;
 cameraDevice=(cameraDevice==CAMERA_INNER)?CAMERA_OUTER:CAMERA_INNER;
 cameraSelect((CameraDevice)cameraDevice);
}
int aetherCameraPreviewCount(void){return previewCount;}
int aetherCameraCaptureCount(void){return captureCount;}
bool aetherMicActive(void){return micRunning;}
int aetherMicPeak(void){return micPeak;}
int aetherMicRms(void){return micRms;}
int aetherCameraPreview(void){
 if(!cameraReady||cameraTransferActive()||ndmaBusy(AETHER_CAMERA_NDMA))return 0;
 if(!cameraSelect((CameraDevice)cameraDevice))return 0;
 if(!cameraStartTransfer(previewBuffer,MCUREG_APT_SEQ_CMD_PREVIEW,AETHER_CAMERA_NDMA))return 0;
 while(cameraTransferActive()||ndmaBusy(AETHER_CAMERA_NDMA))swiWaitForVBlank();
 cameraStopTransfer();ensureCameraDir();writePreviewPPM("fat:/data/AetherMod/camera/preview.ppm");previewCount++;return 1;
}
int aetherCameraCapture(void){
 if(!cameraReady||cameraTransferActive()||ndmaBusy(AETHER_CAMERA_NDMA))return 0;
 if(!cameraSelect((CameraDevice)cameraDevice))return 0;
 if(!cameraStartTransfer(captureBuffer,MCUREG_APT_SEQ_CMD_CAPTURE,AETHER_CAMERA_NDMA))return 0;
 while(cameraTransferActive()||ndmaBusy(AETHER_CAMERA_NDMA))swiWaitForVBlank();
 cameraStopTransfer();ensureCameraDir();
 char path[96];snprintf(path,sizeof(path),"fat:/data/AetherMod/camera/capture_%03d.yuv",captureCount+1);
 FILE *f=fopen(path,"wb");if(!f)return 0;fwrite(captureBuffer,1,sizeof(captureBuffer),f);fclose(f);captureCount++;return 1;
}
int aetherMicStartStop(void){
 if(!micReady)return 0;
 if(micRunning){soundMicOff();micRunning=false;return 1;}
 if(soundMicRecord(micBuffer,sizeof(micBuffer),MicFormat_12Bit,16000,micCallback)){micRunning=true;return 1;}
 return 0;
}
