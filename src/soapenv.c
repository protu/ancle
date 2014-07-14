#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>
#include "ancle.h"

#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)

int searchdevice(xmlNodePtr searchdev, char *productclass)
{
    xmlNodePtr searchquery = NULL;
	searchquery = xmlNewNode(NULL, BAD_CAST "SearchQuery");
	xmlAddChild(searchdev, searchquery);
	xmlNewChild(searchquery, NULL, BAD_CAST "ProductClass", BAD_CAST productclass);

    return(0);
}

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
