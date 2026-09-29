#include "aether_messaging.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>

static char s_text[161];
static unsigned s_len=0, s_cursor=0, s_sent=0, s_received=0, s_queued=0;
static unsigned s_connected=0, s_contact=0, s_platform=0;
static const char* contacts[]={"My iPhone","Android Contact","iPhone Contact","Android Tablet"};
static const char* platforms[]={"iPhone / Apple","Android","iPhone / Apple","Android"};
static const char* colors[]={"BLUE UI","GREEN UI","BLUE UI","GREEN UI"};
static const unsigned color_values[]={0x0070F0u,0x34C759u,0x0070F0u,0x34C759u};
static const char alphabet[]=" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,!?'-_@:/";

void aether_messaging_init(void){memset(s_text,0,sizeof(s_text));s_len=0;s_cursor=0;s_sent=s_received=s_queued=0;s_connected=0;s_contact=0;s_platform=0;}
void aether_messaging_tick(void){if(s_connected && (s_sent+s_received+s_queued)%120u==0u){}}
const char* aether_messaging_contact(void){return contacts[s_contact&3u];}
const char* aether_messaging_platform(void){return platforms[s_platform&3u];}
const char* aether_messaging_color_name(void){return colors[s_platform&3u];}
unsigned aether_messaging_color(void){return color_values[s_platform&3u];}
const char* aether_messaging_text(void){return s_text;}
unsigned aether_messaging_text_len(void){return s_len;}
unsigned aether_messaging_cursor(void){return s_cursor;}
unsigned aether_messaging_sent(void){return s_sent;}
unsigned aether_messaging_received(void){return s_received;}
unsigned aether_messaging_queued(void){return s_queued;}
unsigned aether_messaging_connected(void){return s_connected;}

void aether_messaging_cycle_contact(void){s_contact=(s_contact+1u)&3u;s_platform=s_contact&1u;s_connected=1;}
void aether_messaging_cycle_platform(void){s_platform=(s_platform+1u)&1u;s_connected=1;}
void aether_messaging_cursor_up(void){if(s_cursor>0)s_cursor--;}
void aether_messaging_cursor_down(void){if(s_cursor<sizeof(alphabet)-2)s_cursor++;}
void aether_messaging_backspace(void){if(s_len){s_text[--s_len]=0;}}

void aether_messaging_append_char(char c){if(s_len<160u){s_text[s_len++]=c;s_text[s_len]=0;}}
void aether_messaging_clear(void){s_len=0;s_text[0]=0;s_cursor=0;}

int aether_messaging_send(void){
 if(!s_len || !s_connected)return 0;
 s_sent++;
 /* Transport-neutral queue: the DSi prepares the payload; a real phone gateway
    is required for Internet delivery to Apple/Android messaging networks. */
 if(s_queued<999u)s_queued++;
 aether_messaging_clear();
 return 1;
}
void aether_messaging_simulate_incoming(void){
 const char* p=(s_platform&1u)?"Android":"iPhone";
 const char* msg="Aether test reply: gateway ready.";
 (void)p;
 if(s_len==0){strncpy(s_text,msg,sizeof(s_text)-1);s_text[sizeof(s_text)-1]=0;s_len=(unsigned)strlen(s_text);}
 s_received++;
}
