#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ancle.h"


int main()
{
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

	/**
	 *
	 * TODO: check if the response is valid xml document
	 * and if ti is, then proceed.
	 * Otherwise try few more times before quit
	 *
	**/

	printf("Response:\n%s\n", response.memory);

	int total;
	total = totalRecords(response.memory);
	printf("Total records: %d\n", total);
	devicesFound(response.memory);

	free(response.memory);
	response.memory = NULL;

	return 0;
}

