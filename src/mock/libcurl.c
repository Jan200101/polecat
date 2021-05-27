#include <stdio.h>
#include <stdarg.h>
#include <curl/curl.h>

#include "../defines.h"

char* url = NULL;
void* data = NULL;

#ifdef curl_easy_setopt
#undef curl_easy_setopt
#endif

CURLcode curl_global_init(UNUSED long flags)
{
	puts("[MOCK] curl_global_init(...)");
	return CURLE_OK;
}

CURL* curl_easy_init()
{
	puts("[MOCK] curl_easy_init(...)");
	return NULL;
}

CURLcode curl_easy_setopt(UNUSED CURL *handle, CURLoption option, ...)
{
	puts("[MOCK] curl_easy_setopt(...)");

	va_list arg;
	va_start(arg, option);

	switch (option)
	{
		case CURLOPT_URL:
			url = va_arg(arg, char*);
			printf("CURLOPT_URL\t%s\n", url);
			break;

		case CURLOPT_WRITEDATA:
			data = va_arg(arg, void*);
			printf("CURLOPT_WRITEDATA\t%p\n", data);
			break;

		default:
			break;
	}

	va_end(arg);

	return CURLE_OK;
}

CURLcode curl_easy_perform(UNUSED CURL *easy_handle)
{
	puts("[MOCK] curl_easy_perform(...)");
	return CURLE_OK;
}

#ifdef curl_easy_getinfo
#undef curl_easy_getinfo
#endif

CURLcode curl_easy_getinfo(UNUSED CURL *curl, CURLINFO info, ...)
{
	puts("[MOCK] curl_easy_getinfo(...)");

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
	puts("[MOCK] curl_easy_strerror(...)");
	return "error";
}

void curl_easy_cleanup(CURL *handle)
{
	puts("[MOCK] curl_easy_cleanup(...)");
}

void curl_global_cleanup()
{
	puts("[MOCK] curl_global_cleanup(...)");
}