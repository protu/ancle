#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "ancle.h"
#include <string.h>

#define nameCh (char *)name
#define keyCh (char *)key

// Create xmlDoc from character response
xmlDocPtr responseToDoc(char *response);
static xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath);

void freeDevice(Device *deviceList)
{
	Device *firstDevice = deviceList;
	while(deviceList->oui)
	{
		free(deviceList->oui);
		free(deviceList->productclass);
		free(deviceList->serialnumber);
		free(deviceList->description);
		deviceList++;
	}
	free(firstDevice);
}

void printDevice(Device *deviceList)
{
	while(deviceList->oui)
	{
		printf("OUI: %s\n", deviceList->oui);
		printf("Product class: %s\n", deviceList->productclass);
		printf("Serial: %s\n", deviceList->serialnumber);
		printf("Description: %s\n", deviceList->description);
		printf("---------------\n");
		deviceList++;
	}
}

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
		result = NULL;
	}
	else
		fprintf(stderr, "No result\n");

	xmlFreeDoc(rspDoc);
	rspDoc=NULL;

	return(devicesNr);
}


static void parseDevices(xmlDoc *doc, xmlNode * a_node, Device * deviceList)
{
	xmlNode *cur_node = a_node;
	xmlChar *key = NULL;
	xmlChar *name = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (xmlStrEqual(cur_node->name, BAD_CAST "ResultFields"))
			{
				if (deviceList->oui != NULL)
					deviceList++;
			}
			else if (xmlStrEqual(cur_node->name, BAD_CAST "Name"))
			{
				name = xmlNodeListGetString(doc, cur_node->children, 1);
			}
			else if (xmlStrEqual(cur_node->name, BAD_CAST "Value"))
			{
				key = xmlNodeListGetString(doc, cur_node->children, 1);
				if (strcmp(nameCh, "OUI") == 0)
				{
					if ((deviceList->oui = malloc(strlen(keyCh)+1)) == NULL)
					{
						fprintf(stderr, "Not enough memory");
					}
					strcpy(deviceList->oui, keyCh);
				}
				else if (strcmp(nameCh, "SerialNumber") == 0)
				{
					if ((deviceList->serialnumber = malloc(strlen(keyCh)+1)) == NULL)
					{
						fprintf(stderr, "Not enough memory");
					}
					strcpy(deviceList->serialnumber, keyCh);
				} 
				else if (strcmp(nameCh, "ProductClass") == 0)
				{
					if ((deviceList->productclass = malloc(strlen(keyCh)+1)) == NULL)
					{
						fprintf(stderr, "Not enough memory");
					}
					strcpy(deviceList->productclass, keyCh);
				} 
				else if (strcmp(nameCh, "Description") == 0)
				{
					if ((deviceList->description = malloc(strlen(keyCh)+1)) == NULL)
					{
						fprintf(stderr, "Not enough memory");
					}
					strcpy(deviceList->description, keyCh);
				} 
				if(key)
				{
					xmlFree(key);
					key = NULL;
				}
				if(name)
				{
					xmlFree(name);
					name = NULL;
				}
			}
		}

		parseDevices(doc, cur_node->children, deviceList);
	}
}

int devicesFound(char *response, int total)
{
	Device *deviceList = NULL;
	xmlNodePtr curNode ;

	xmlDocPtr rspDoc = responseToDoc(response);
	if (rspDoc == NULL)
	{
		fprintf(stderr, "Response is not parsed succesfully. \n");
		return 1;
	}

	curNode = xmlDocGetRootElement(rspDoc);
	if (curNode == NULL)
	{
		fprintf(stderr, "Empty response\n");
		xmlFreeDoc(rspDoc);
		return 0;
	}

	// Allocate one device more, so the last one will be NULL
	if((deviceList = calloc(sizeof(Device), (total+1) * sizeof(Device))) == NULL)
	{
		fprintf(stderr, "Not enough memory to store devices\n");
		return 1;
	}
	parseDevices(rspDoc, curNode, deviceList);
	xmlFreeDoc(rspDoc);
	rspDoc = NULL;

	printDevice(deviceList);
	freeDevice(deviceList);

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

static xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath)
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
