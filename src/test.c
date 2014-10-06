#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "acs_innbox.h"

int main(void) {

	int state = 0;

	const char *productClass = PRODUCT_CLASS;
	char *request = NULL;

	if (asprintf(&request, "%s%s%s", SEARCH_HEAD, productClass, SEARCH_TAIL)
			== -1)
		fprintf(stderr, "Can't allocate memory for request string\n");

	printf("Request:\n%s\n", request);

	free(request);

	return state;
}

