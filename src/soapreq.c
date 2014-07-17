#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>
#include "ancle.h"

#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#define DEV_PARAM_SIZE 4

int searchdevice(xmlNodePtr searchdev, char *productclass)
{
    xmlNodePtr searchquery = NULL;
	searchquery = xmlNewNode(NULL, BAD_CAST "SearchQuery");
	xmlAddChild(searchdev, searchquery);
	xmlNewChild(searchquery, NULL, BAD_CAST "ProductClass", BAD_CAST productclass);

    return(0);
}

int customsearch(xmlNodePtr customsearch, DevicePtr dev)
{
	// Select fields to display
	xmlNodePtr selectfields = xmlNewNode(NULL, BAD_CAST "SelectFields");
	xmlAddChild(customsearch, selectfields);
	xmlNodePtr selectfield = NULL;
	int i;
	const char *devParams[DEV_PARAM_SIZE] = {"OUI", "SerialNumber", "ProductClass", "Description"};
	for (i=0; i<DEV_PARAM_SIZE; i++)
	{
		selectfield = xmlNewChild(selectfields, NULL, BAD_CAST "SelectField", NULL);
		xmlNewChild(selectfield, NULL, BAD_CAST "Type", BAD_CAST "Device");
		xmlNewChild(selectfield, NULL, BAD_CAST "Name", BAD_CAST devParams[i]);
	}

	// Select fiels to search
	xmlNodePtr criteria = xmlNewNode(NULL, BAD_CAST "Criteria");
	xmlNodePtr required = NULL;
	xmlNodePtr any = NULL;
	if (dev->oui)
	{
		required = xmlNewChild(criteria, NULL, BAD_CAST "Required", NULL);
		any = xmlNewChild(required, NULL, BAD_CAST "Any", NULL);
		xmlNewChild(any, NULL, BAD_CAST "Type", BAD_CAST "Device");
		xmlNewChild(any, NULL, BAD_CAST "Name", BAD_CAST "OUI");
		xmlNewChild(any, NULL, BAD_CAST "Operator", BAD_CAST "=");
		xmlNewChild(any, NULL, BAD_CAST "Value", BAD_CAST dev->oui);
	}
	if (dev->productclass)
	{
		required = xmlNewChild(criteria, NULL, BAD_CAST "Required", NULL);
		any = xmlNewChild(required, NULL, BAD_CAST "Any", NULL);
		xmlNewChild(any, NULL, BAD_CAST "Type", BAD_CAST "Device");
		xmlNewChild(any, NULL, BAD_CAST "Name", BAD_CAST "ProductClass");
		xmlNewChild(any, NULL, BAD_CAST "Operator", BAD_CAST "=");
		xmlNewChild(any, NULL, BAD_CAST "Value", BAD_CAST dev->productclass);
	}
	if (dev->serialnumber)
	{
		required = xmlNewChild(criteria, NULL, BAD_CAST "Required", NULL);
		any = xmlNewChild(required, NULL, BAD_CAST "Any", NULL);
		xmlNewChild(any, NULL, BAD_CAST "Type", BAD_CAST "Device");
		xmlNewChild(any, NULL, BAD_CAST "Name", BAD_CAST "SerialNumber");
		xmlNewChild(any, NULL, BAD_CAST "Operator", BAD_CAST "=");
		xmlNewChild(any, NULL, BAD_CAST "Value", BAD_CAST dev->serialnumber);
	}
	if (dev->description)
	{
		required = xmlNewChild(criteria, NULL, BAD_CAST "Required", NULL);
		any = xmlNewChild(required, NULL, BAD_CAST "Any", NULL);
		xmlNewChild(any, NULL, BAD_CAST "Type", BAD_CAST "Device");
		xmlNewChild(any, NULL, BAD_CAST "Name", BAD_CAST "Description");
		xmlNewChild(any, NULL, BAD_CAST "Operator", BAD_CAST "LIKE");
		xmlNewChild(any, NULL, BAD_CAST "Value", BAD_CAST dev->description);
	}
	
	xmlAddChild(customsearch, criteria);

	return(0);
}


/**
 * This funcition conbine soap envelope with desired soap 
 * action as defined for ACSLite
**/

char *soapreq(DevicePtr dev)
{
	char *request = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr envelope = NULL, body = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    envelope = xmlNewNode(NULL, BAD_CAST "Envelope");

	xmlNsPtr nssoapenv = xmlNewNs(envelope, BAD_CAST"http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soapenv");
	xmlNsPtr nsnbi = xmlNewNs(envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4", BAD_CAST "nbi");
	xmlSetNs(envelope, nssoapenv);

    xmlDocSetRootElement(doc, envelope);
    xmlNewChild(envelope, NULL, BAD_CAST "Header", NULL);
    body = xmlNewNode(nssoapenv, BAD_CAST "Body");
	xmlAddChild(envelope, body);

	xmlNodePtr sd = xmlNewNode(nsnbi, BAD_CAST "CustomSearch");
	customsearch(sd, dev);
	if (sd)
		xmlAddChild(body, sd);
	else
		fprintf(stderr, "No body defined\n");

	xmlBufferPtr buf = xmlBufferCreate();
	if(buf)
	{
		xmlSaveCtxtPtr ctx = xmlSaveToBuffer(buf, "UTF-8", XML_SAVE_FORMAT|XML_SAVE_NO_DECL);
		if(ctx)
		{
			xmlSaveDoc(ctx, doc);
			xmlSaveClose(ctx);
		}
		request = malloc(strlen((const char*)xmlBufferContent(buf))+1);
		strcpy(request, (const char *)xmlBufferContent(buf));
		xmlBufferFree(buf);
	}

    /*free the document */
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return(request);
}

/**
 * This funcition creates SOAP envelope to be used with all
 * requests
 *
 * It sould be chaged to work that way
**/

char *soapenv(DevicePtr dev)
{
	char *request = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr envelope = NULL, body = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    envelope = xmlNewNode(NULL, BAD_CAST "Envelope");

	xmlNsPtr nssoapenv = xmlNewNs(envelope, BAD_CAST"http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soapenv");
	xmlNsPtr nsnbi = xmlNewNs(envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4", BAD_CAST "nbi");
	xmlSetNs(envelope, nssoapenv);

    xmlDocSetRootElement(doc, envelope);
    xmlNewChild(envelope, NULL, BAD_CAST "Header", NULL);
    body = xmlNewNode(nssoapenv, BAD_CAST "Body");
	xmlAddChild(envelope, body);

	xmlNodePtr sd = xmlNewNode(nsnbi, BAD_CAST "SearchDevices");
	searchdevice(sd, dev->productclass);
	if (sd)
		xmlAddChild(body, sd);

	xmlBufferPtr buf = xmlBufferCreate();
	if(buf)
	{
		xmlSaveCtxtPtr ctx = xmlSaveToBuffer(buf, "UTF-8", XML_SAVE_NO_DECL);
		if(ctx)
		{
			xmlSaveDoc(ctx, doc);
			xmlSaveClose(ctx);
		}
		request = malloc(strlen((const char*)xmlBufferContent(buf))+1);
		strcpy(request, (const char *)xmlBufferContent(buf));
		xmlBufferFree(buf);
	}

    /*free the document */
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return(request);
}

#else
int main(void) {
    fprintf(stderr, "tree support not compiled in\n");
    exit(1);
}
#endif
