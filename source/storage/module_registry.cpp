#include "module_registry.h"
#include <fat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
namespace aether::storage {
static const char* kNames[]={
"Quantum Core","YHWH Codex","Animal AI","RF Passive Lab","TinySA Lab",
"Calculator","DAW / Game Studio","DSP / FFT","Telemetry","AI Home",
"Network Gateway","Phone Link","Media Studio","Sensor Hub","Data Vault",
"File Browser","Haptic Lab","Accessibility","Power Lab","Control Lab",
"Diagnostics","Aether Bot","General Settings","Event Log","Notes",
"Clock","About","Safety Center","Universe Home"
};
static const char* kClass[]={
"CORE","CODEX","AI","RADIO","LAB","MATH","STUDIO","DSP","SYSTEM","AI",
"NETWORK","LINK","MEDIA","SENSOR","DATA","FILES","INPUT","ACCESS","POWER","CONTROL",
"DIAGNOSTICS","AGENT","SETTINGS","LOG","PIM","SYSTEM","INFO","SAFETY","FRONTEND"
};
int moduleCount(){return (int)(sizeof(kNames)/sizeof(kNames[0]));}
const char* moduleName(int i){return i>=0&&i<moduleCount()?kNames[i]:"";}
const char* moduleClass(int i){return i>=0&&i<moduleCount()?kClass[i]:"";}
bool writeRegistry(){
    FILE* f=fopen("REVF/STORAGE/MANIFEST/MODULES.TXT","w");
    if(!f) return false;
    fprintf(f,"AETHER MODULE REGISTRY\nFORMAT=1\nCOUNT=%d\n",moduleCount());
    for(int i=0;i<moduleCount();++i) fprintf(f,"%02d|%s|%s\n",i+1,moduleClass(i),moduleName(i));
    fclose(f); return true;
}
}
