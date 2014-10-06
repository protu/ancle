#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ancle.h"

/**
 * This is the test program for parsing soap search request.
 * If there is an argument than this is a file with previously
 * cached search result and it will parsed.
 * Otherwise, send search request to the ACS server and parse it
 **/

int main(int argc, char **argv) {
	// Point to given response string

	char *productClass = "Innbox V51 Home Gateway AnnexB";
	char *oui = "00D050";
	char *serialnumber = "1234567";
	char *description = "dario@snifko - 6661400 - BSA_HH_IPTV";
	DevicePtr dev = malloc(strlen(productClass) + strlen(oui));
	if (dev == NULL)
		fprintf(stderr, "Malloc failed at dev\n");

	dev->productclass = productClass;
	dev->oui = oui;
	dev->serialnumber = serialnumber;
	dev->description = description;

	char *request = NULL;
	request = soapSearch(dev);
	printf("Search request:\n%s\n", request);
	request = soapRegister(dev);
	printf("Register request:\n%s\n", request);
	request = soapDelete(dev);
	printf("Delete request:\n%s\n", request);
	free(dev);

	free(request);

	return 0;
}

