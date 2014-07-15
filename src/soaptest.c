#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ancle.h"

int main()
{
	char *productClass = "Innbox V51 Home Gateway AnnexB";
	DevicePtr dev = malloc(strlen(productClass));
	if(dev == NULL)
		fprintf(stderr, "Malloc failed at dev\n");

	dev->productclass = productClass;
	char *request = soapreq(dev);
	printf("Request:\n%s\n", request);

	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size=0;

	callCurl(request, &response);
	printf("Response:\n%s\n", response.memory);
	free(response.memory);
	free(request);
	return 0;
}
