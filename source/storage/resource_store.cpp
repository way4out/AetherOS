#include "resource_store.h"
#include <fat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace aether::storage {
static bool gReady=false;
static const u32 kCacheBytes=44u*1024u*1024u;

static bool dir(const char* p){
    struct stat st{};
    return stat(p,&st)==0 && (st.st_mode&S_IFDIR);
}
static bool mk(const char* p){
    if(dir(p)) return true;
    if(mkdir(p,0777)==0) return true;
    return dir(p);
}
static bool dirs(){
    const char* d[]={"REVF","REVF/STORAGE","REVF/STORAGE/MANIFEST","REVF/STORAGE/CACHE"};
    for(unsigned i=0;i<sizeof(d)/sizeof(d[0]);++i) if(!mk(d[i])) return false;
    return true;
}
bool writeManifest(){
    if(!gReady || !dirs()) return false;
    FILE* f=fopen("REVF/STORAGE/MANIFEST/INDEX.TXT","w");
    if(!f) return false;
    fprintf(f,"AETHER RESOURCE VAULT\n");
    fprintf(f,"FORMAT=1\n");
    fprintf(f,"MEDIA_PROFILE=512GB\n");
    fprintf(f,"APPLICATION_TARGET=400GB\n");
    fprintf(f,"CACHE=44MiB\n");
    fprintf(f,"DIRECTORIES=CORE,QUANTUM,SOUND,DSP,LAB,AI,NETWORK,PROJECTS,SAMPLES,PRESETS,PLUGINS,BENCH,LOGS,RECOVERY\n");
    fprintf(f,"POLICY=NEVER_REQUIRE_CACHE_FOR_BOOT\n");
    fclose(f);
    return true;
}
bool ensureCache(){
    if(!gReady || !dirs()) return false;
    FILE* f=fopen("REVF/STORAGE/CACHE/AETHER44.BIN","rb");
    if(f){ fclose(f); return verifyCache(); }
    f=fopen("REVF/STORAGE/CACHE/AETHER44.BIN","wb");
    if(!f) return false;
    if(fseek(f,(long)kCacheBytes-1L,SEEK_SET)!=0 || fputc(0,f)==EOF){
        fclose(f); remove("REVF/STORAGE/CACHE/AETHER44.BIN"); return false;
    }
    fclose(f);
    return true;
}
bool verifyCache(){
    FILE* f=fopen("REVF/STORAGE/CACHE/AETHER44.BIN","rb");
    if(!f) return false;
    if(fseek(f,0,SEEK_END)!=0){ fclose(f); return false; }
    long n=ftell(f); fclose(f);
    return n==(long)kCacheBytes;
}
bool initialize(){
    gReady=fatInitDefault();
    if(!gReady) return false;
    if(!dirs() || !writeManifest()) return false;
    // Cache allocation is best-effort; boot never depends on it.
    (void)ensureCache();
    return true;
}
bool vaultReady(){ return gReady && verifyCache(); }
u32 cacheMiB(){ return 44; }
}
