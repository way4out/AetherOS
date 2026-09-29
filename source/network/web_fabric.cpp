#include "web_fabric.h"
#include "network_fabric.h"
#include <fat.h>
#include <stdio.h>
#include <string.h>
namespace { unsigned ticks=0; bool ready=false;
bool starts(const char*s,const char*p){return s&&p&&strncmp(s,p,strlen(p))==0;}
}
namespace aether::web {
void init(){ticks=0;ready=fatInitDefault();}
void tick(){++ticks;}
UriInfo inspect(const char* uri){
 UriInfo x={UNKNOWN,false,false,false};
 if(starts(uri,"https://")){x={HTTPS,true,true,false};}
 else if(starts(uri,"http://")){x={HTTP,true,false,false};}
 else if(starts(uri,"oeql://")){x={OEQL,true,true,true};}
 else if(starts(uri,"onion://")||starts(uri,"http://*.onion")||starts(uri,"https://*.onion")){x={ONION,true,true,true};}
 return x;
}
static bool append(const char* kind,const char* uri,const char* a,const char*b){
 if(!ready) return false;
 FILE*f=fopen("REVF/NETWORK/WEBQUEUE.TXT","a"); if(!f)return false;
 fprintf(f,"%s|%s|%s|%s\n",kind?kind:"REQ",uri?uri:"",a?a:"",b?b:""); fclose(f); return true;
}
bool queueRequest(const char* uri,const char* method,const char* body){
 UriInfo x=inspect(uri); if(!x.routable) return false;
 return append(x.scheme==OEQL?"OEQL":(x.scheme==ONION?"ONION":"WEB"),uri,method,body);
}
bool queuePush(const char* uri,const char* payload){
 // "push" is an application/network operation only; RF interference/jamming is never enabled.
 return queueRequest(uri,"PUSH",payload);
}
bool queueWalletConnect(const char* uri,const char* session){
 return queueRequest(uri,"WALLET_CONNECT",session);
}
}
