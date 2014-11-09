/*
 * Copyright 2014 Dario Protulipac
 *
 * This file is part of Ancle.
 *
 * Ancle is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Ancle is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Ancle.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>
#include "ancle.h"

#define DEV_PARAM_SIZE 4

static char *
docToChar (xmlDocPtr doc)
{
  char *request = NULL;
  xmlBufferPtr buf = xmlBufferCreate ();
  if (buf)
    {
      xmlSaveCtxtPtr ctx = xmlSaveToBuffer (buf, "UTF-8",
					    XML_SAVE_FORMAT |
					    XML_SAVE_NO_DECL);
      if (ctx)
	{
	  xmlSaveDoc (ctx, doc);
	  xmlSaveClose (ctx);
	}
      request = malloc (strlen ((const char *) xmlBufferContent (buf)) + 1);
      strcpy (request, (const char *) xmlBufferContent (buf));
      xmlBufferFree (buf);
    }
  else
    {
      fprintf (stderr, "Error creating xml buffer\n");
    }

  xmlFreeDoc (doc);
  xmlCleanupParser ();
  xmlMemoryDump ();
  return (request);
}

static int
registerdevice (xmlNodePtr registerdevice, DevicePtr dev)
{
  xmlNodePtr deviceID = xmlNewNode (NULL, BAD_CAST "DeviceID");
  xmlAddChild (registerdevice, deviceID);
  xmlNewChild (deviceID, NULL, BAD_CAST "OUI", BAD_CAST dev->oui);
  xmlNewChild (deviceID, NULL, BAD_CAST "ProductClass",
	       BAD_CAST dev->productclass);
  xmlNewChild (deviceID, NULL, BAD_CAST "SerialNumber",
	       BAD_CAST dev->serialnumber);

  if (dev->description)
    {
      xmlNodePtr
	deviceProperties = xmlNewNode (NULL, BAD_CAST "DeviceProperties");
      xmlAddChild (registerdevice, deviceProperties);
      xmlNewChild (deviceProperties, NULL, BAD_CAST "Description",
		   BAD_CAST dev->description);
    }

  return (0);
}

static int
deletedevice (xmlNodePtr registerdevice, DevicePtr dev)
{
  xmlNodePtr deviceID = xmlNewNode (NULL, BAD_CAST "DeviceID");
  xmlAddChild (registerdevice, deviceID);
  xmlNewChild (deviceID, NULL, BAD_CAST "OUI", BAD_CAST dev->oui);
  xmlNewChild (deviceID, NULL, BAD_CAST "ProductClass",
	       BAD_CAST dev->productclass);
  xmlNewChild (deviceID, NULL, BAD_CAST "SerialNumber",
	       BAD_CAST dev->serialnumber);

  return (0);
}

static int
setDeviceFlag (xmlNodePtr registerdevice, DevicePtr dev, flagPtr flag)
{
  xmlNodePtr deviceID = xmlNewNode (NULL, BAD_CAST "DeviceID");
  xmlAddChild (registerdevice, deviceID);
  xmlNewChild (deviceID, NULL, BAD_CAST "OUI", BAD_CAST dev->oui);
  xmlNewChild (deviceID, NULL, BAD_CAST "ProductClass",
	       BAD_CAST dev->productclass);
  xmlNewChild (deviceID, NULL, BAD_CAST "SerialNumber",
	       BAD_CAST dev->serialnumber);

  xmlNodePtr flagValueList = xmlNewNode (NULL, BAD_CAST "FlagValueList");
  xmlAddChild (registerdevice, flagValueList);
  xmlNodePtr flagValueStruct = xmlNewNode (NULL, BAD_CAST "FlagValueStruct");
  xmlNewChild (flagValueStruct, NULL, BAD_CAST "Name", BAD_CAST flag->name);
  xmlNewChild (flagValueStruct, NULL, BAD_CAST "Value", BAD_CAST flag->value);
  xmlAddChild (flagValueList, flagValueStruct);

  return (0);
}

static int
customsearch (xmlNodePtr customsearch, DevicePtr dev)
{
// Select fields to display
  xmlNodePtr selectfields = xmlNewNode (NULL, BAD_CAST "SelectFields");
  xmlAddChild (customsearch, selectfields);
  xmlNodePtr selectfield = NULL;
  int i;
  const char *devParams[DEV_PARAM_SIZE] =
    { "OUI", "SerialNumber", "ProductClass",
    "Description"
  };
  for (i = 0; i < DEV_PARAM_SIZE; i++)
    {
      selectfield =
	xmlNewChild (selectfields, NULL, BAD_CAST "SelectField", NULL);
      xmlNewChild (selectfield, NULL, BAD_CAST "Type", BAD_CAST "Device");
      xmlNewChild (selectfield, NULL, BAD_CAST "Name", BAD_CAST devParams[i]);
    }

  // Select fiels to search
  xmlNodePtr criteria = xmlNewNode (NULL, BAD_CAST "Criteria");
  xmlNodePtr required = NULL;
  xmlNodePtr any = NULL;
  if (dev->oui)
    {
      required = xmlNewChild (criteria, NULL, BAD_CAST "Required", NULL);
      any = xmlNewChild (required, NULL, BAD_CAST "Any", NULL);
      xmlNewChild (any, NULL, BAD_CAST "Type", BAD_CAST "Device");
      xmlNewChild (any, NULL, BAD_CAST "Name", BAD_CAST "OUI");
      xmlNewChild (any, NULL, BAD_CAST "Operator", BAD_CAST "LIKE");
      xmlNewChild (any, NULL, BAD_CAST "Value", BAD_CAST dev->oui);
    }
  if (dev->productclass)
    {
      required = xmlNewChild (criteria, NULL, BAD_CAST "Required", NULL);
      any = xmlNewChild (required, NULL, BAD_CAST "Any", NULL);
      xmlNewChild (any, NULL, BAD_CAST "Type", BAD_CAST "Device");
      xmlNewChild (any, NULL, BAD_CAST "Name", BAD_CAST "ProductClass");
      xmlNewChild (any, NULL, BAD_CAST "Operator", BAD_CAST "LIKE");
      xmlNewChild (any, NULL, BAD_CAST "Value", BAD_CAST dev->productclass);
    }
  if (dev->serialnumber)
    {
      required = xmlNewChild (criteria, NULL, BAD_CAST "Required", NULL);
      any = xmlNewChild (required, NULL, BAD_CAST "Any", NULL);
      xmlNewChild (any, NULL, BAD_CAST "Type", BAD_CAST "Device");
      xmlNewChild (any, NULL, BAD_CAST "Name", BAD_CAST "SerialNumber");
      xmlNewChild (any, NULL, BAD_CAST "Operator", BAD_CAST "LIKE");
      xmlNewChild (any, NULL, BAD_CAST "Value", BAD_CAST dev->serialnumber);
    }
  if (dev->description)
    {
      required = xmlNewChild (criteria, NULL, BAD_CAST "Required", NULL);
      any = xmlNewChild (required, NULL, BAD_CAST "Any", NULL);
      xmlNewChild (any, NULL, BAD_CAST "Type", BAD_CAST "Device");
      xmlNewChild (any, NULL, BAD_CAST "Name", BAD_CAST "Description");
      xmlNewChild (any, NULL, BAD_CAST "Operator", BAD_CAST "LIKE");
      xmlNewChild (any, NULL, BAD_CAST "Value", BAD_CAST dev->description);
    }

  xmlAddChild (customsearch, criteria);

  return (0);
}

static xmlDocPtr
soapStart ()
{
  xmlDocPtr doc = NULL;
  xmlNodePtr envelope = NULL, body = NULL;

  doc = xmlNewDoc (BAD_CAST "1.0");
  envelope = xmlNewNode (NULL, BAD_CAST "Envelope");

  xmlNsPtr
    nssoapenv =
    xmlNewNs (envelope, BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/",
	      BAD_CAST "soapenv");
  xmlSetNs (envelope, nssoapenv);

  xmlDocSetRootElement (doc, envelope);
  xmlNewChild (envelope, NULL, BAD_CAST "Header", NULL);
  body = xmlNewNode (nssoapenv, BAD_CAST "Body");
  xmlAddChild (envelope, body);

  return doc;
}

static void
addSearch (xmlDocPtr doc, DevicePtr dev)
{
  xmlNodePtr envelope = xmlDocGetRootElement (doc);
  xmlNodePtr body = xmlLastElementChild (envelope);
  xmlNsPtr
    nsnbi =
    xmlNewNs (envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4",
	      BAD_CAST "nbi");
  xmlNodePtr sd = xmlNewNode (nsnbi, BAD_CAST "CustomSearch");
  customsearch (sd, dev);
  if (sd)
    xmlAddChild (body, sd);
  else
    fprintf (stderr, "No body defined\n");
}

static void
addRegister (xmlDocPtr doc, DevicePtr dev)
{
  xmlNodePtr envelope = xmlDocGetRootElement (doc);
  xmlNodePtr body = xmlLastElementChild (envelope);
  xmlNsPtr
    nsnbi =
    xmlNewNs (envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4",
	      BAD_CAST "nbi");
  xmlNodePtr sd = xmlNewNode (nsnbi, BAD_CAST "RegisterDevice");
  registerdevice (sd, dev);
  xmlAddChild (body, sd);
}

static void
addDelete (xmlDocPtr doc, DevicePtr dev)
{
  xmlNodePtr envelope = xmlDocGetRootElement (doc);
  xmlNodePtr body = xmlLastElementChild (envelope);
  xmlNsPtr
	nsnbi =
	xmlNewNs (envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4",
			  BAD_CAST "nbi");
  xmlNodePtr sd = xmlNewNode (nsnbi, BAD_CAST "DeleteDevice");
  deletedevice (sd, dev);
  xmlAddChild (body, sd);
}

static void
addSetFlag (xmlDocPtr doc, DevicePtr dev, flagPtr flag)
{
  xmlNodePtr envelope = xmlDocGetRootElement (doc);
  xmlNodePtr body = xmlLastElementChild (envelope);
  xmlNsPtr
	nsnbi =
	xmlNewNs (envelope, BAD_CAST "urn:www.acslite.com/nbi:1.4", BAD_CAST "nbi");
  xmlNodePtr sd = xmlNewNode (nsnbi, BAD_CAST "SetDeviceFlags");
  setDeviceFlag(sd, dev, flag);
  xmlAddChild (body, sd);
}

char *
soapSearch (DevicePtr dev)
{
  xmlDocPtr doc = NULL;
  doc = soapStart ();
  addSearch (doc, dev);
  return docToChar (doc);
}

char *
soapRegister (DevicePtr dev)
{
  xmlDocPtr doc = NULL;
  doc = soapStart ();
  addRegister (doc, dev);
  return docToChar (doc);
}

char *
soapDelete (DevicePtr dev)
{
  xmlDocPtr doc = NULL;
  doc = soapStart ();
  addDelete (doc, dev);
  return docToChar (doc);
}

char *
soapAddFlag (DevicePtr dev, flagPtr flag)
{
  xmlDocPtr doc = NULL;
  doc = soapStart();
  addSetFlag(doc, dev, flag);
  return docToChar (doc);
}

