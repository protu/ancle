#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include "ancle.h"



int getdevices(Device *device)
{

/*
 * Construct search SOAP request for sending to ACSLite's NBI
 */

	if (device->serialnumber != NULL && device->productclass == NULL)
	{
		printf("Search by serial number is not yet implemented\n");
		return 0;
	}
	else if (device->serialnumber != NULL)
		printf("Search by serial number is not yet implemented, only product class will be used\n");

	char *request = soapreq(device);
	device = NULL;

	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size=0;
	
	callCurl(request, &response);
	if(request)
	{
		free(request);
		request = NULL;
	}

	char *responsePtr = response.memory;
	int total;

	total = totalRecords(responsePtr);
	printf("Total records: %d\n", total);
	devicesFound(responsePtr, total);

	free(responsePtr);
	responsePtr = NULL;
	xmlCleanupParser();

	return 0;
}

