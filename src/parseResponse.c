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

/**
 * @file parseResponse.c
 * @brief Functions for parsing responses from ACS server
 *
 * Collections of functions that will parse responses that ACS
 * server return as answer to given request.
 *
 */
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "ancle.h"

#define nameCh (char *)name
#define keyCh (char *)key

xmlDocPtr responseToDoc (char *response);
static xmlXPathObjectPtr getnodeset (xmlDocPtr doc, const xmlChar * xpath);

void
freeDevice (Device * deviceList)
{
  Device *firstDevice = deviceList;
  while (deviceList->oui)
    {
      free (deviceList->oui);
      free (deviceList->productclass);
      free (deviceList->serialnumber);
      free (deviceList->description);
      deviceList++;
    }
  free (firstDevice);
}

void
printDevice (Device * deviceList)
{
  int i = 0;
  printf ("\n--------------------------------------------------------------------------------\n");
  while (deviceList->oui)
    {
	  printf ("%d.\n", ++i);
      printf ("  OUI:           %s\n", deviceList->oui);
      printf ("  Product class: %s\n", deviceList->productclass);
      printf ("  Serial:        %s\n", deviceList->serialnumber);
      printf ("  Description:   %s\n", deviceList->description);
      printf ("--------------------------------------------------------------------------------\n");
      deviceList++;
    }
}

/**
 * Function will parse number of devices from
 * given SOAP response.
 * If response is set to NULL, then it will return 
 * last paresed, nuber of devies
 */

int
totalRecords (char *response)
{

  static int devicesNr;

  if (response == NULL)
	return devicesNr;
  else
	devicesNr = 0;

  xmlDocPtr rspDoc = responseToDoc (response);

  // Define search string
  xmlChar *xpathTotal = (xmlChar *) "//TotalRecords";

  xmlXPathObjectPtr result;
  xmlChar *keyword;
  int i;
  xmlNodeSetPtr nodeset;

  result = getnodeset (rspDoc, xpathTotal);
  if (result)
    {
      nodeset = result->nodesetval;
      for (i = 0; i < nodeset->nodeNr; i++)
	{
	  keyword = xmlNodeListGetString (rspDoc,
					  nodeset->nodeTab[i]->xmlChildrenNode, 1);
	  devicesNr = atoi ((char *) keyword);
	  xmlFree (keyword);
	}
      xmlXPathFreeObject (result);
      result = NULL;
    }
  else
    fprintf (stderr, "No result\n");

  xmlFreeDoc (rspDoc);
  rspDoc = NULL;

  return (devicesNr);
}

static xmlChar *
checkResult(xmlDocPtr rspDoc, const xmlChar *xpathExpr)
{
  xmlXPathObjectPtr result;
  xmlChar *keyword = NULL;
  int i;
  xmlNodeSetPtr nodeset;
  result = getnodeset (rspDoc, xpathExpr);
  if (result)
	{
	  nodeset = result->nodesetval;
	  for (i = 0; i < nodeset->nodeNr; i++)
		{
		  keyword = xmlNodeListGetString (rspDoc,
										  nodeset->nodeTab[i]->xmlChildrenNode, 1);
		}
	  xmlXPathFreeObject (result);
	  result = NULL;
	}
  if(keyword == NULL || strlen((char *)keyword) == 0)
	return NULL;
  else
	return keyword;
}


xmlChar *
createResult (char *response)
{

  xmlDocPtr rspDoc = responseToDoc (response);

  // Define search strings
  const xmlChar *xpathSuccess = BAD_CAST "//Success";
  const xmlChar *xpathFaultDetail = BAD_CAST "//detail";

  xmlChar *keyword = NULL;

  keyword = checkResult(rspDoc, xpathSuccess);
  if (keyword)
	{
	  xmlFreeDoc(rspDoc);
	  rspDoc = NULL;
	  xmlCleanupParser();
	  return keyword;
	}

  keyword = checkResult(rspDoc, xpathFaultDetail);
  if (keyword)
	{
	  xmlFreeDoc(rspDoc);
	  rspDoc = NULL;
	  xmlCleanupParser();
	  return keyword;
	}

  xmlFreeDoc(rspDoc);
  rspDoc = NULL;
  xmlCleanupParser();
  return keyword;
}

/**
 * @brief Fill array of devices from XML response
 * @param doc XML document
 * @param a_node starting node in XML document
 * @prarm deviceList dynamic array that will be filled
 *
 * Used to parse given XML document to fill the device list array.
 * Function use recursion to go deep into document and pickup 
 * all device's parametes.
 *
 */
static void
parseDevices (xmlDoc * doc, xmlNode * a_node, Device * deviceList)
{
  xmlNode *cur_node = a_node;
  xmlChar *key = NULL;
  xmlChar *name = NULL;

  for (cur_node = a_node; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  if (xmlStrEqual (cur_node->name, BAD_CAST "ResultFields"))
	    {
	      if (deviceList->oui != NULL)
		deviceList++;
	    }
	  else if (xmlStrEqual (cur_node->name, BAD_CAST "Name"))
	    {
	      name = xmlNodeListGetString (doc, cur_node->children, 1);
	    }
	  else if (xmlStrEqual (cur_node->name, BAD_CAST "Value"))
	    {
	      key = xmlNodeListGetString (doc, cur_node->children, 1);
	      if (strcmp (nameCh, "OUI") == 0 && key)
		{
		  if ((deviceList->oui = malloc (strlen (keyCh) + 1)) == NULL)
		    {
		      fprintf (stderr, "Not enough memory");
		    }
		  strcpy (deviceList->oui, keyCh);
		}
	      else if (strcmp (nameCh, "SerialNumber") == 0 && key)
		{
		  if ((deviceList->serialnumber = malloc (strlen (keyCh) + 1))
		      == NULL)
		    {
		      fprintf (stderr, "Not enough memory");
		    }
		  strcpy (deviceList->serialnumber, keyCh);
		}
	      else if (strcmp (nameCh, "ProductClass") == 0 && key)
		{
		  if ((deviceList->productclass = malloc (strlen (keyCh) + 1))
		      == NULL)
		    {
		      fprintf (stderr, "Not enough memory");
		    }
		  strcpy (deviceList->productclass, keyCh);
		}
	      else if (strcmp (nameCh, "Description") == 0 && key)
		{
		  if ((deviceList->description = malloc (strlen (keyCh) + 1))
		      == NULL)
		    {
		      fprintf (stderr, "Not enough memory");
		    }
		  strcpy (deviceList->description, keyCh);
		}
	      if (key)
		{
		  xmlFree (key);
		  key = NULL;
		}
	      if (name)
		{
		  xmlFree (name);
		  name = NULL;
		}
	    }
	}

      parseDevices (doc, cur_node->children, deviceList);
    }
}

Device *
devicesFound (char *response, int total)
{
  Device *deviceList = NULL;
  xmlNodePtr curNode;

  xmlDocPtr rspDoc = responseToDoc (response);
  if (rspDoc == NULL)
    {
      fprintf (stderr, "Response is not parsed succesfully. \n");
      return NULL;
    }

  curNode = xmlDocGetRootElement (rspDoc);
  if (curNode == NULL)
    {
      fprintf (stderr, "Empty response\n");
      xmlFreeDoc (rspDoc);
      return NULL;
    }

  // Allocate one device more, so the last one will be NULL
  if ((deviceList = calloc (sizeof (Device), (total + 1) * sizeof (Device)))
      == NULL)
    {
      fprintf (stderr, "Not enough memory to store devices\n");
      return NULL;
    }
  parseDevices (rspDoc, curNode, deviceList);
  xmlFreeDoc (rspDoc);
  rspDoc = NULL;
  xmlCleanupParser ();

  return (deviceList);
}

/**
 * @brief Create xmlDoc from character response
 */
xmlDocPtr
responseToDoc (char *response)
{
  xmlDocPtr doc;
  xmlChar *soapResponse;
  soapResponse = xmlCharStrdup (response);

  if (soapResponse == NULL)
    {
      fprintf (stderr, "Respose parsed to NULL\n");
      return 0;
    }

  doc = xmlParseDoc (soapResponse);
  if (doc == NULL)
    {
      fprintf (stderr, "Empty document\n");
      xmlFreeDoc (doc);
      return 0;
    }
  xmlFree (soapResponse);
  soapResponse = NULL;

  return doc;
}

/**
 * @brief search document using xpath and return pointer
 * @param doc document to search
 * @param xpath search xpath expression
 *
 */
static xmlXPathObjectPtr
getnodeset (xmlDocPtr doc, const xmlChar * xpath)
{
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext (doc);
  if (context == NULL)
    {
      printf ("Error in xmlXPathNewContext\n");
      return NULL;
    }

  result = xmlXPathEvalExpression (xpath, context);
  xmlXPathFreeContext (context);
  if (result == NULL)
    {
      printf ("Error in xmlXPathEvalExpression\n");
      return NULL;
    }

  if (xmlXPathNodeSetIsEmpty (result->nodesetval))
    {
      xmlXPathFreeObject (result);
      return NULL;
    }

  return result;
}
