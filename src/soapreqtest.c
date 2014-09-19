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

int main(int argc, char **argv)
{
	// Point to given response string

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

	free(request);
	
	return 0;
}

