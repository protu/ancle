#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "ancle.h"


static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL)
	{
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(mem->memory, contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}

int callCurl(char *request, struct MemoryStruct *response)
{

	int c;
	c = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (c != 0)
	{
		fprintf(stderr, "Curl global error: %d\n", c);
		return c;
	}

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl)
	{
//		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  
		curl_easy_setopt(curl, CURLOPT_URL, ACS_NBI_URL);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_USERPWD, ACS_NBI_USERPWD);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			return (1);
		}
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return(0);
}
