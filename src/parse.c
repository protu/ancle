#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "ancle.h"

// Create xmlDoc from character response
xmlDocPtr responseToDoc(char *response);
xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath);

int totalRecords(char * response)
{

	xmlDocPtr rspDoc = responseToDoc(response);
	
	// Define search string
	xmlChar *xpathTotal = (xmlChar *) "//TotalRecords";

	xmlXPathObjectPtr result;
	xmlChar *keyword;
	int i, devicesNr;
	xmlNodeSetPtr nodeset;
	result = getnodeset (rspDoc, xpathTotal);
	if (result)
	{
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++)
		{
			keyword = xmlNodeListGetString(rspDoc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			devicesNr = atoi((char *)keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject(result);
	}
	else
		fprintf(stderr, "No result\n");

	xmlFreeDoc(rspDoc);
	rspDoc=NULL;

	return(devicesNr);
}

int devicesFound(char *response)
{
	xmlDocPtr rspDoc = responseToDoc(response);
	
	// Define search string
	xmlChar *xpathTotal = (xmlChar *) "//ResultField";

	xmlXPathObjectPtr result;
	int i;
	xmlNodeSetPtr nodeset;
	result = getnodeset (rspDoc, xpathTotal);
	if (result)
	{
		nodeset = result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++)
		{
			printf("Node content:\n%s\n", (char *)nodeset->nodeTab[i]->content );
		}
		xmlXPathFreeObject(result);
	}
	else
		fprintf(stderr, "No result\n");

	xmlFreeDoc(rspDoc);

	return(0);
}


xmlDocPtr responseToDoc(char *response)
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
