
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json.h>

#include "net.h"
#include "common.h"

static size_t memoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    uint8_t* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static int xferinfo(UNUSED void *p, curl_off_t dltotal, curl_off_t dlnow, UNUSED curl_off_t ultotal, UNUSED curl_off_t ulnow)
{
    curl_off_t progress = 0;
    if (dltotal != 0)
        progress = ((float)dlnow / dltotal) * 100;

    fprintf(stderr, "\rProgress: %3" CURL_FORMAT_CURL_OFF_T "%%", progress);

    if (progress == 100) fprintf(stderr, "\r");

    return 0;
}

struct MemoryStruct* downloadToRam(const char* URL, long noprogress)
{
    CURL* curl_handle;
    CURLcode res = CURLE_OK;

    if (!isatty(STDERR_FILENO))
        noprogress = 1L;

    struct MemoryStruct* chunk = malloc(sizeof(struct MemoryStruct));

    if (chunk)
    {
        // if we managed to allocate the chunk lets assume we can allocate at least 1 byte
        chunk->memory = malloc(1);
        chunk->size = 0;

        curl_global_init(CURL_GLOBAL_ALL);

        curl_handle = curl_easy_init();

        curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, memoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, xferinfo);
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, noprogress);

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
    else
    {
        fprintf(stderr, "Unable to allocate memory\n");
    }

    return chunk;
}

struct json_object* fetchJSON(const char* URL)
{
    struct MemoryStruct* chunk = downloadToRam(URL, 1L);

    struct json_object* json = NULL;

    if (chunk)
    {
        json = json_tokener_parse((char*)chunk->memory);

        free(chunk->memory);
        free(chunk);

    }

    return json;
}
