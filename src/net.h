#ifndef NET_H
#define NET_H

#include <json.h>

size_t WriteMemoryCallback(void*, size_t, size_t, void*);
int downloadFile(const char*, const char*);
struct json_object* fetchJSON(const char*);

#endif