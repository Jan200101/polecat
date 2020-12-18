#ifndef NET_H
#define NET_H

#include <curl/curl.h>
#include <json.h>

#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T

size_t WriteMemoryCallback(void*, size_t, size_t, void*);
struct MemoryStruct* downloadToRam(const char* URL, long);
struct json_object* fetchJSON(const char*);

#endif
