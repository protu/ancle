/**
 * @file getdevices.c
 * @brief Hold basic actions for searching, registring and deleting devices
 * 
 * Functions for searching, regestring and deleteing devices wich will based on action,
 * find desired devices on ACS server and display it on standard output or delelete it.
 * 
 * It will also register new device, that is defined in Device structure.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "ancle.h"

/**
 * Send search request to the server and return 
 * array of devices that match criteria
 * @param  device reference device that will used as search pattern
 */

static Device *
finddevices(Device *device) {

    char *request = soapSearch(device);
    device = NULL;

    struct MemoryStruct response;
    response.memory = malloc(1);
    response.size = 0;

    if (!callCurl(request, &response))
    {
        fprintf(stderr, "Curl returned NULL\n");
        return NULL;
    }

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

/**
 * @brief Find devices based on parameter and display it
 * @param device reference device that will used as search pattern
 */

int
getdevices(Device *device) {
    Device *listdevices = NULL;
    if((listdevices = finddevices(device)))
    {
        printDevice(listdevices);
        freeDevice(listdevices);
    }
    return 0;
}

/**
 * Delete devices based on search paramter
 * delete is yet not completed and will act as
 * getdevices function
 * @param device reference device that will used as search pattern
 */

int
deldevices(Device *device) {
    Device *listdevices = NULL;
    if((listdevices = finddevices(device)))
    {
        printDevice(listdevices);
        freeDevice(listdevices);
    }
    return 0;
}

/**
 * Used to register new device into ACS server
 * @param device must contain fully specified device
 */

int
regdevice(Device *device) {
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

