/*
 * Copyright 2014 Dario Protulipac
 *
 * This file is part of Ancle.
 *
 * Ancle is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Ancle is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Ancle.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "ancle.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *) userp;
	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL)
	{
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	if (verbose)
	{
		printf("Memory size: %d\n", (int)mem->size);
		printf("Memory value: %s\n", mem->memory);
	}
	return realsize;
}

int callCurl(char *request, struct MemoryStruct *response)
{
	int c;

	if (verbose)
	{
	  printf ("%s\n", "Request:");
	  printf ("%s\n", request);
	}

	acs *serverdata = NULL;
	if ((serverdata=setACS()) == NULL)
	{
		fprintf(stderr, "Curl can't get ACS server data\n");
		return 0;
	}

	c = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (c != 0)
	{
		fprintf(stderr, "Curl global error: %d\n", c);
		return c;
	}

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		if (verbose)
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 2L);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		curl_easy_setopt(curl, CURLOPT_URL, serverdata->url);
		curl_easy_setopt(curl, CURLOPT_USERNAME, serverdata->user );
		curl_easy_setopt(curl, CURLOPT_PASSWORD, serverdata->pass );
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) response);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			return (0);
		}
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return (1);
}
