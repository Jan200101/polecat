
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "net.h"
#include "common.h"
 
size_t memoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct* mem = (struct MemoryStruct*)userp;
 
  uint8_t* ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    puts("out of memory");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

struct MemoryStruct* downloadToRam(const char* URL)
{
    CURL* curl_handle;
    CURLcode res;

    struct MemoryStruct* chunk = malloc(sizeof(struct MemoryStruct));

    if (chunk)
    {
        chunk->memory = malloc(1);
        chunk->size = 0;

        curl_global_init(CURL_GLOBAL_ALL);

        curl_handle = curl_easy_init();

        curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, memoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl_handle);

        long http_code = 0;
        curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

        if(res != CURLE_OK) {
            puts(curl_easy_strerror(res));
            return NULL;
        }
        else if (http_code != 200)
        {
#ifdef DEBUG
            printf("HTTP Error %li\n", http_code);
#endif
            return NULL;
        }

        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
    }

    return chunk;
}

void downloadFile(const char* URL, const char* path)
{
    struct MemoryStruct* chunk = downloadToRam(URL);

    if (chunk)
    {
        FILE* file = fopen(path, "wb");
        if (file)
        {
            fwrite(chunk->memory, chunk->size, 1, file);
            fclose(file);
        }

        free(chunk->memory);
        free(chunk);
    }
}

struct json_object* fetchJSON(const char* URL)
{
    struct MemoryStruct* chunk = downloadToRam(URL);

    struct json_object* json = NULL;

    if (chunk)
    {
        json = json_tokener_parse((char*)chunk->memory);

        free(chunk->memory);
        free(chunk);

    }

    return json;
}