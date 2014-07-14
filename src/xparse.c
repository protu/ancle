#define _GNU_SOURCE

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

xmlDocPtr getdoc (char *docname)
{
	xmlDocPtr doc;
	doc = xmlParseFile(docname);

	if (doc == NULL)
	{
		fprintf(stderr, "Document not parsed successfully. \n");
		return NULL;
	}

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

int main(int argc, char **argv)
{
	char *docname;
	xmlDocPtr doc;
	xmlChar *xpathSerial = (xmlChar *) "//SerialNumber";
	xmlChar *xpathTotal = (xmlChar *) "//TotalRecords";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *keyword;
	int devicesNr = 0;

	if (argc <= 1)
	{
		printf("Usage: %s docname\n", argv[0]);
		return(0);
	}

	docname = argv[1];
	doc = getdoc(docname);
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
	return(1);
}

