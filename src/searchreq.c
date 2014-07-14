#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>

#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)

int main(int argc, char **argv)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr envelope = NULL, body = NULL, searchdev = NULL, searchquery = NULL;

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");

	/**
	 * SOAP envelope for request
	 */

    envelope = xmlNewNode(NULL, BAD_CAST "Envelope");

	xmlNsPtr nssoapenv = xmlNewNs(envelope, BAD_CAST"http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soapenv");
	xmlNsPtr nsnbi = xmlNewNs(envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4", BAD_CAST "nbi");
	xmlSetNs(envelope, nssoapenv);

    xmlDocSetRootElement(doc, envelope);
    xmlNewChild(envelope, NULL, BAD_CAST "Header", NULL);
    body = xmlNewNode(nssoapenv, BAD_CAST "Body");
	xmlAddChild(envelope, body);

	/**
	 * NBI part for device search
	 */

	searchdev = xmlNewNode(nsnbi, BAD_CAST "SearchDevices");
	xmlAddChild(body, searchdev);
	searchquery = xmlNewNode(NULL, BAD_CAST "SearchQuery");
	xmlAddChild(searchdev, searchquery);
	xmlNewChild(searchquery, NULL, BAD_CAST "ProductClass", BAD_CAST "Innbox V51");


	xmlBufferPtr buf = xmlBufferCreate();
	xmlSaveCtxtPtr ctx;

	ctx = xmlSaveToBuffer(buf, "UTF-8", XML_SAVE_FORMAT|XML_SAVE_NO_DECL);

	xmlSaveDoc(ctx, doc);
	xmlSaveClose(ctx);
	puts((const char*)xmlBufferContent(buf));

    /*free the document */
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return(0);
}

#else
int main(void) {
    fprintf(stderr, "tree support not compiled in\n");
    exit(1);
}
#endif
