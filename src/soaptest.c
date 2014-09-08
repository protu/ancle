#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ancle.h"
#include <libxml/parser.h>


/**
 * This is the test program for parsing soap search request.
 * If there is an argument than this is a file with previously
 * cached search result and it will parsed.
 * Otherwise, send search request to the ACS server and parse it
**/

int main(int argc, char **argv)
{
	// Point to given response string
	char *responsePtr = NULL;

	if (argc == 2)
	{
		printf("Parsing file %s\n", argv[1]);

		FILE *in_file = fopen(argv[1], "r");
		if (in_file == NULL)
		{
			fprintf(stderr, "Can't open file %s\n", argv[1]);
			return 1;
		}

		size_t input_file_size, read_file_size;

		fseek(in_file, 0, SEEK_END);
		input_file_size = ftell(in_file);
		rewind(in_file);

		responsePtr = malloc((input_file_size + 1) * sizeof(char));
		read_file_size = fread(responsePtr, sizeof(char), input_file_size, in_file);
		if (read_file_size != input_file_size)
		{
			fprintf(stderr, "File is not read correctly!\n");
			return 1;
		}

		fclose(in_file);
		responsePtr[input_file_size] = '\0';
	}
	else
	{
		/** 
		 * Define device structure from which the search string will be created.
		**/

		char *productClass = "Innbox V51 Home Gateway AnnexB";
		char *oui= "00D050";
		DevicePtr dev = malloc(strlen(productClass) + strlen(oui));
		if(dev == NULL)
			fprintf(stderr, "Malloc failed at dev\n");

		dev->productclass = productClass;
		dev->oui = oui;
		dev->serialnumber = NULL;
		dev->description = NULL;

		char *request = soapreq(dev);
		free(dev);

		printf("Request:\n%s\n", request);

		struct MemoryStruct response;
		response.memory = malloc(1);
		response.size=0;

		callCurl(request, &response);
		free(request);
		
		responsePtr = response.memory;
	}

	/**
	 *
	 * TODO: check if the response is valid xml document
	 * and if ti is, then proceed.
	 * Otherwise try few more times before quit
	 *
	**/

	// printf("Response:\n%s\n", responsePtr);

	int total;
	total = totalRecords(responsePtr);
	printf("Total records: %d\n", total);
	devicesFound(responsePtr, total);

	free(responsePtr);
	responsePtr = NULL;
	xmlCleanupParser();
	return 0;
}

