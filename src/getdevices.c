#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "ancle.h"

#include <libxml/parser.h>


xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL)
	{
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}

	if(xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("No result\n");
		return NULL;
	}

	return result;
}

xmlDocPtr prepareResponse(char *response)
{
	xmlDocPtr doc;
	xmlChar *soapResponse;
	soapResponse = xmlCharStrdup(response);
	if(soapResponse == NULL)
	{
			fprintf(stderr, "Respose parsed to NULL\n");
			return 0;
	}

	doc = xmlParseDoc(soapResponse);
	if(doc == NULL)
	{
			fprintf(stderr, "Empty document\n");
			xmlFreeDoc(doc);
			return 0;
	}
	xmlFree(soapResponse);

	return doc;
}



int getdevices(char * productClass, char * serialNumber)
{

/*
 * Construct search SOAP request for sending to ACSLite's NBI
 */

	xmlDocPtr doc;	
	xmlChar *xpathTotal = (xmlChar *) "//TotalRecords";
	xmlChar *xpathSerial = (xmlChar *) "//SerialNumber";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;

	int i;
	xmlChar *keyword;
	int devicesNr = 0;

	if (serialNumber != NULL && productClass == NULL)
	{
		printf("Search by serial number is not yet implemented\n");
		return 0;
	}
	else if (serialNumber != NULL)
		printf("Search by serial number is not yet implemented, only product class will be used\n");
	
	DevicePtr dev = malloc(sizeof(*productClass) + sizeof(*serialNumber));
	dev->productclass = productClass;

	char *request = soapenv(dev);
	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size=0;
	
	callCurl(request, &response);
	if(request)
	{
		free(request);
		request = NULL;
	}

	doc = prepareResponse(response.memory);

	if(response.memory)
	{
		free(response.memory);
		response.memory = NULL;
	}

	// Get total number of devices

	result = getnodeset (doc, xpathTotal);

	if (result)
	{
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++)
		{
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			printf("TotalRecords: %s\n", keyword);
			devicesNr = atoi((char *)keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject(result);
	}
	else
		fprintf(stderr, "No result\n");

	/**
	 * From here, parse result
	 */

	// Get devices's serial numbers
	
	result = getnodeset(doc, xpathSerial);
	char **serials;
	if ((serials = malloc(devicesNr * sizeof(char *))) == NULL)
	{
		fprintf(stderr, "Malloc failed at serials\n");
		return(-1);
	}

	if (result)
	{
		nodeset = result->nodesetval;
		for (i = 0; i < devicesNr; i++)
		{
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			char *kw = (char *)keyword;
			if ((serials[i] = malloc(strlen(kw)+1)) == NULL)
			{
				fprintf(stderr, "Malloc failed at serials[%d]\n", i);
				return(-1);
			}
			strcpy(serials[i], kw);
			xmlFree(keyword);
		}
		xmlXPathFreeObject(result);
	}
	else
		fprintf(stderr, "No result\n");

	for (i=0; i < devicesNr; i++)
		printf("%d: %s\n", i+1, serials[i]);

	for (i=0; i < devicesNr; i++)
		free(serials[i]);
	free(serials);
	xmlFreeDoc(doc);
	xmlCleanupParser();


	return 0;
}

