#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>

#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)

int main(int argc, char **argv)
{
	xmlNodePtr searchdev = NULL, searchquery = NULL;

	xmlNsPtr nsnbi = xmlNewNs(searchdev, BAD_CAST "urn:www.acslite.com/nbi:1.4", BAD_CAST "nbi");
	xmlSetNs(searchdev, nsnbi);
	searchdev = xmlNewNode(nsnbi, BAD_CAST "SearchDevices");
	searchquery = xmlNewNode(NULL, BAD_CAST "SearchQuery");
	xmlAddChild(searchdev, searchquery);
	xmlNewChild(searchquery, NULL, BAD_CAST "ProductClass", BAD_CAST "Innbox V51");

	xmlBufferPtr buf = xmlBufferCreate();
	if(buf)
	{
		xmlSaveCtxtPtr ctx = xmlSaveToBuffer(buf, "UTF-8", XML_SAVE_FORMAT);
		if (ctx)
		{
			xmlSaveTree(ctx, searchdev);
			xmlSaveClose(ctx);
		}
		printf("%s", ((const char*)xmlBufferContent(buf)));
		xmlBufferFree(buf);
	}

	/*free the document */
	xmlFreeNs(nsnbi);
	xmlCleanupParser();
	return(0);
}

#else
int main(void) {
	fprintf(stderr, "tree support not compiled in\n");
	exit(1);
}
#endif
