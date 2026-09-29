#pragma once
#include <nds.h>
namespace aether::web {
enum Scheme : u8 { HTTP, HTTPS, OEQL, ONION, UNKNOWN };
struct UriInfo { Scheme scheme; bool routable; bool encrypted; bool requiresRelay; };
void init(); void tick();
UriInfo inspect(const char* uri);
bool queueRequest(const char* uri, const char* method, const char* body);
bool queuePush(const char* uri, const char* payload);
bool queueWalletConnect(const char* uri, const char* session);
}
