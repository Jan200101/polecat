#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <curl/curl.h>

#include "../defines.h"

#include "wine_mock_tar_xz.h"
#include "wine_json.h"

typedef size_t (*callback_t)(const void*, size_t, size_t, void*);
typedef int (*xfercallback_t)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

#ifndef NDEBUG
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

callback_t callbackfunc = NULL;
xfercallback_t xfercallbackfunc = NULL;
char* url = NULL;
void* data = NULL;
long noprogress = 0;


#ifdef curl_easy_setopt
#undef curl_easy_setopt
#endif

CURLcode curl_global_init(UNUSED long flags)
{
    debug_printf("[MOCK] %s(...)\n", __func__);
    return CURLE_OK;
}

CURL* curl_easy_init()
{
    debug_printf("[MOCK] %s(...)\n", __func__);
    return NULL;
}

CURLcode curl_easy_setopt(UNUSED CURL *handle, CURLoption option, ...)
{
    debug_printf("[MOCK] %s(...)\n", __func__);

    va_list arg;
    va_start(arg, option);

    switch (option)
    {
        case CURLOPT_URL:
            url = va_arg(arg, char*);
            debug_printf("CURLOPT_URL\t%s\n", url);
            break;

        case CURLOPT_WRITEDATA:
            data = va_arg(arg, void*);
            debug_printf("CURLOPT_WRITEDATA\t%p\n", data);
            break;

        case CURLOPT_WRITEFUNCTION:
            callbackfunc = va_arg(arg, callback_t);
            debug_printf("CURLOPT_WRITEFUNCTION\n");
            break;

        case CURLOPT_XFERINFOFUNCTION:
            xfercallbackfunc = va_arg(arg, xfercallback_t);
            debug_printf("CURLOPT_XFERINFOFUNCTION\n");
            break;

        case CURLOPT_NOPROGRESS:
            noprogress = va_arg(arg, long);
            debug_printf("CURLOPT_NOPROGRESS\t%li\n", noprogress);
            break;

        default:
            break;
    }

    va_end(arg);

    return CURLE_OK;
}

CURLcode curl_easy_perform(UNUSED CURL *easy_handle)
{
    const void* output = NULL;
    size_t output_size = 0;

    debug_printf("[MOCK] %s(...)\n", __func__);
    if (!strcmp(url, WINE_API))
    {
        output = wine_json;
        output_size = wine_json_size;
    }
    else if (!strcmp(url, "mockurl"))
    {
        output = wine_mock_tar_xz;
        output_size = wine_mock_tar_xz_size;
    }

    if (output && data && callbackfunc)
    {
        callbackfunc(output, 1, output_size, data);

        if (xfercallbackfunc && !noprogress)
        {
            xfercallbackfunc(data, (curl_off_t)output_size, (curl_off_t)output_size, 0, 0);
        }

        return CURLE_OK;
    }

    return CURLE_UNSUPPORTED_PROTOCOL;
}

#ifdef curl_easy_getinfo
#undef curl_easy_getinfo
#endif

CURLcode curl_easy_getinfo(UNUSED CURL *curl, CURLINFO info, ...)
{
    debug_printf("[MOCK] %s(...)\n", __func__);

    if (info == CURLINFO_RESPONSE_CODE)
    {
        va_list arg;
        va_start(arg, info);
        long* http_code = va_arg(arg, long*);
        *http_code = 200;
        va_end(arg);
    }


    return CURLE_OK;
}

const char* curl_easy_strerror(UNUSED CURLcode error)
{
    debug_printf("[MOCK] %s(...)\n", __func__);
    return __func__;;
}

void curl_easy_cleanup(CURL *handle)
{
    debug_printf("[MOCK] %s(...)\n", __func__);
}

void curl_global_cleanup()
{
    debug_printf("[MOCK] %s(...)\n", __func__);
}
