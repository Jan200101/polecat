#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <curl/curl.h>
#include <assert.h>

#include "../defines.h"

#ifdef WINE_ENABLED
#include "wine_mock_tar_xz.h"
#include "wine_json.h"
#endif

typedef size_t (*callback_t)(const void*, size_t, size_t, void*);
typedef int (*xfercallback_t)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

#ifndef NDEBUG
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif
#define mockup_printf(FORMAT, ...) debug_printf(" mock: %s("FORMAT")\n", __func__, __VA_ARGS__)
#define mockup_print() mockup_printf("%s", "")

#define CURL_MOCK_HANDLE (void*)0xabababab

callback_t callbackfunc = NULL;
xfercallback_t xfercallbackfunc = NULL;
char* url = NULL;
void* data = NULL;
long noprogress = 0;


#ifdef curl_easy_setopt
#undef curl_easy_setopt
#endif

CURLcode curl_global_init(long flags)
{
    mockup_printf("flags=0x%lx", flags);
    return CURLE_OK;
}

CURL* curl_easy_init()
{
    mockup_print();
    return CURL_MOCK_HANDLE;
}

CURLcode curl_easy_setopt(CURL *handle, CURLoption option, ...)
{
    assert(handle == CURL_MOCK_HANDLE);

    va_list arg;
    va_start(arg, option);

    switch (option)
    {
        case CURLOPT_URL:
            url = va_arg(arg, char*);
            mockup_printf("handle=%p, option=CURLOPT_URL, url=\"%s\"", handle, url);
            break;

        case CURLOPT_WRITEDATA:
            data = va_arg(arg, void*);
            mockup_printf("handle=%p, option=CURLOPT_WRITEDATA, data_address=%p", handle, data);
            break;

        case CURLOPT_WRITEFUNCTION:
            callbackfunc = va_arg(arg, callback_t);
            mockup_printf("handle=%p, option=CURLOPT_WRITEFUNCTION, writefunction=%p", handle, FPTR(callbackfunc));
            break;

        case CURLOPT_XFERINFOFUNCTION:
            xfercallbackfunc = va_arg(arg, xfercallback_t);
            mockup_printf("handle=%p, option=CURLOPT_XFERINFOFUNCTION, xferinfofunction=%p", handle, FPTR(xfercallbackfunc));
            break;

        case CURLOPT_NOPROGRESS:
            noprogress = va_arg(arg, long);
            mockup_printf("handle=%p, option=CURLOPT_NOPROGRESS, noprogress=%li", handle, noprogress);
            break;

        default:
            mockup_printf("handle=%p, option=0x%x", handle, option);
            break;
    }

    va_end(arg);

    return CURLE_OK;
}

CURLcode curl_easy_perform(CURL *handle)
{
    const void* output = NULL;
    size_t output_size = 0;

    mockup_printf("handle=%p", handle);
    assert(handle == CURL_MOCK_HANDLE);
#ifdef WINE_ENABLED
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
#endif

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

CURLcode curl_easy_getinfo(UNUSED CURL *handle, CURLINFO info, ...)
{
    mockup_printf("handle=%p", handle);
    assert(handle == CURL_MOCK_HANDLE);

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
    mockup_printf("error=0x%x", error);
    return __func__;;
}

void curl_easy_cleanup(CURL *handle)
{
    assert(handle == CURL_MOCK_HANDLE);
    mockup_printf("handle=%p", handle);
}

void curl_global_cleanup()
{
    mockup_print();
}
