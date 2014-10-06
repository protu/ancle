#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "ancle.h"

static Device *finddevices(Device *device) {

	/*
	 * Construct search SOAP request for sending to ACSLite's NBI
	 */

	char *request = soapSearch(device);
	device = NULL;

	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size = 0;

	callCurl(request, &response);
	if (request) {
		free(request);
		request = NULL;
	}

	char *responsePtr = response.memory;
	int total;

	if (verbose)
		printf("Response:\n%s\n", responsePtr);

	total = totalRecords(responsePtr);
	printf("Total records: %d\n", total);
	Device *listdevices = NULL;
	listdevices = devicesFound(responsePtr, total);

	free(responsePtr);
	responsePtr = NULL;

	return listdevices;
}

int getdevices(Device *device) {
	Device *listdevices = NULL;
	listdevices = finddevices(device);
	printDevice(listdevices);
	freeDevice(listdevices);
	return 0;
}

int deldevices(Device *device) {
	Device *listdevices = NULL;
	listdevices = finddevices(device);
	printDevice(listdevices);
	freeDevice(listdevices);
	return 0;
}

int regdevice(Device *device) {
	char *request = soapRegister(device);
	device = NULL;

	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size = 0;

	callCurl(request, &response);
	if (request) {
		free(request);
		request = NULL;
	}

	char *responsePtr = response.memory;
	if (verbose)
		printf("Response:\n%s\n", responsePtr);

	if (createResult(responsePtr) == 0)
		printf("Device successfully registered\n");
	else
		printf("Error registering device\n");

	free(responsePtr);
	responsePtr = NULL;
	return 0;
}

