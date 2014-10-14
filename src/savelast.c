#include "stdio.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlsave.h"

#if DEBUG
#include "libxml/xmlmemory.h"
#endif

int
main (int argc, char **argv)
{
  int fail = -1;
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s xmlfile\n", argv[0]);
      return 2;
    }
#if DEBUG
  xmlMemSetup (xmlMemFree, xmlMemMalloc, xmlMemRealloc, xmlMemoryStrdup);
#endif

  xmlInitParser ();

  {
    xmlDocPtr doc = xmlReadFile (argv[1], 0, 0);
    if (doc)
      {
	xmlNodePtr last = xmlLastElementChild (xmlDocGetRootElement (doc));
	if (last)
	  {
	    xmlBufferPtr buf = xmlBufferCreate ();
	    if (buf)
	      {
		xmlSaveCtxtPtr savectx = xmlSaveToBuffer (buf, 0,
							  XML_SAVE_FORMAT);
		if (savectx)
		  {
		    xmlSaveTree (savectx, last);
		    fail = 0 > xmlSaveClose (savectx);
		    puts ((const char *) xmlBufferContent (buf));
		  }
		xmlBufferFree (buf);
	      }
	  }
	xmlFreeDoc (doc);
      }
  }

#if DEBUG
  xmlCleanupParser ();
  {
    int leaked_memory = xmlMemUsed ();
    if (leaked_memory)
      {
	xmlMemDisplay (stderr);
	fprintf (stderr, "Leaked %d bytes\n", leaked_memory);
      }
    else
      fprintf (stderr, "No memory leaked\n");
  }
#endif
  return fail;
}
