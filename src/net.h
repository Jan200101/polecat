#ifndef NET_H
#define NET_H

#include <curl/curl.h>
#include <json.h>

void net_init();
void net_deinit();

struct MemoryStruct* downloadToRam(const char* URL, int);
void downloadToFile(const char*, const char*, int);
struct json_object* fetchJSON(const char*);

#endif
