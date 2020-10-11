#ifndef NET_H
#define NET_H

#include <json-c/json.h>

size_t WriteMemoryCallback(void*, size_t, size_t, void*);
struct MemoryStruct* downloadToRam(const char* URL);
void downloadFile(const char*, const char*);
struct json_object* fetchJSON(const char*);

#endif