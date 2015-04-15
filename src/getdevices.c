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
 * @file getdevices.c
 * @brief Hold basic actions for searching, registring and deleting devices
 * 
 * Functions for searching, regestring and deleteing devices wich will based on action,
 * find desired devices on ACS server and display it on standard output or delelete it.
 * 
 * It will also register new device, that is defined in Device structure.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ancle.h"

/**
 * Send search request to the server and return 
 * array of devices that match criteria
 * @param  device reference device that will used as search pattern
 */

static Device *
finddevices (Device *device, flag *deviceFlag)
{

  char *request = soapSearch (device, deviceFlag);
  device = NULL;

  struct MemoryStruct response;
  response.memory = malloc (1);
  response.size = 0;

  if (!callCurl (request, &response))
    {
      fprintf (stderr, "Curl returned NULL\n");
      return NULL;
    }

  if (request)
    {
      free (request);
      request = NULL;
    }

  char *responsePtr = response.memory;
  int total;

  if (verbose)
    printf ("Response:\n%s\n", responsePtr);

  total = totalRecords (responsePtr);
  printf ("Total records: %d\n", total);
  Device *listdevices = NULL;

  if (total != 0)
    {
      listdevices = devicesFound (responsePtr, total);
    }

  free (responsePtr);
  responsePtr = NULL;

  return listdevices;
}

/**
 * @brief Find devices based on parameter and display it
 * @param device reference device that will used as search pattern
 */

int
getdevices (Device *device, flag *deviceFlag)
{
  Device *listdevices = NULL;
  if ((listdevices = finddevices (device, deviceFlag)))
    {
      printDevice (listdevices);
      freeDevice (listdevices);
    }
  return 0;
}

static int
rmDevice (Device *device)
{
  char *request = soapDelete (device);
  xmlChar *keyword = NULL;
  struct MemoryStruct response;
  response.memory = malloc (1);
  response.size = 0;

  if (!callCurl (request, &response))
    {
      fprintf (stderr, "Curl returned NULL\n");
      return 0;
    }

  if (request)
    {
      free (request);
      request = NULL;
    }

  char *responsePtr = response.memory;
  if (verbose)
    printf ("Response:\n%s\n", responsePtr);

  if ((keyword = createResult (responsePtr)) == NULL)
    {
      fprintf (stderr, "Undefined response from server\n");
      free (response.memory);
      response.memory = NULL;
      responsePtr = NULL;
      return 0;
    }

  if (strcmp ("true", (char *) keyword) == 0)
    {
      printf ("%s - %s - %s - %s - DELETED\n", device->description,
	      device->serialnumber, device->oui, device->productclass);
    }
  else
    {
      printf ("%s\n", (char *) keyword);
    }

  xmlFree (keyword);
  keyword = NULL;
  free (response.memory);
  response.memory = NULL;
  responsePtr = NULL;
  return 0;
}

/**
 * Call function to delete each individual device in the list.
 * I will also add nubering in front of each device
 * @ param deviceList pointer to array of devices
 */

void
rmDevices (Device *deviceList)
{
  int i = 0;

  while (deviceList->oui)
    {
      printf ("%d. ", ++i);
      rmDevice (deviceList++);
    }
}

/**
 * Delete devices based on search paramter
 * delete is yet not completed and will act as
 * getdevices function
 * @param device reference device that will used as search pattern
 */

int
deldevices (Device *device)
{
  Device *listdevices = NULL;
  listdevices = finddevices (device, NULL);
  if (listdevices)
    {
      printf ("Are you sure, you wan't to delete %d device(s)? (y/N):",
	      totalRecords (NULL));
      int c = 0;

      if (yes)
	{
	  c = 'y';
	  printf ("%c\n", c);
	}
      else
	c = getchar ();

      if (c == (int) 'y' || c == (int) 'Y')
	{
	  rmDevices (listdevices);
	  freeDevice (listdevices);
	}
      else
	printf ("Aborting!\n");

    }
  return 0;
}

/**
 * Used to register new device into ACS server
 * @param device must contain fully specified device
 */

int
regdevice (Device *device)
{
  char *request = soapRegister (device);
  xmlChar *keyword = NULL;

  struct MemoryStruct response;
  response.memory = malloc (1);
  response.size = 0;

  if (!callCurl (request, &response))
    {
      fprintf (stderr, "Curl returned NULL\n");
      return 0;
    }

  if (request)
    {
      free (request);
      request = NULL;
    }

  char *responsePtr = response.memory;
  if (verbose)
    printf ("Response:\n%s\n", responsePtr);

  if ((keyword = createResult (responsePtr)) == NULL)
    {
      fprintf (stderr, "Undefined response from server\n");
      free (response.memory);
      response.memory = NULL;
      responsePtr = NULL;
      return 0;
    }

  if (strcmp ("true", (char *) keyword) == 0)
    {
      printf ("Device successfully registered\n");
    }
  else
    {
      printf ("%s\n", (char *) keyword);
    }

  xmlFree (keyword);
  keyword = NULL;
  free (response.memory);
  response.memory = NULL;
  responsePtr = NULL;
  return 0;
}

/**
 * @brief Add flag to device
 * @param device individual device to add flag
 * @param deviceFlag flag structure
 */
addFlag (Device *device, flag *deviceFlag)
{
  char *request = soapAddFlag (device, deviceFlag);
  xmlChar *keyword = NULL;
  struct MemoryStruct response;
  response.memory = malloc (1);
  response.size = 0;

  if (!callCurl (request, &response))
    {
      fprintf (stderr, "Curl returned NULL\n");
      return 0;
    }

  if (request)
    {
      free (request);
      request = NULL;
    }

  char *responsePtr = response.memory;
  if (verbose)
    printf ("Response:\n%s\n", responsePtr);

  if ((keyword = createResult (responsePtr)) == NULL)
    {
      fprintf (stderr, "Undefined response from server\n");
      free (response.memory);
      response.memory = NULL;
      responsePtr = NULL;
      return 0;
    }

  if (strcmp ("true", (char *) keyword) == 0)
    {
      printf ("%s - %s - %s - %s - FLAGED\n", device->description,
	      device->serialnumber, device->oui, device->productclass);
    }
  else
    {
      printf ("%s\n", (char *) keyword);
    }

  xmlFree (keyword);
  keyword = NULL;
  free (response.memory);
  response.memory = NULL;
  responsePtr = NULL;
  return 0;
}
/**
 * @brief Set or get value of the flag from found devices
 * @param device reference device that will used as search pattern
 * @param deviceFlag search for flag is value in not defined. Otherwise set value for named flag.
 *
 */
int
flagdevices (Device *device, flag *deviceFlag)
{
  Device *listdevices = NULL;
  listdevices = finddevices (device, NULL);
  if (listdevices)
    {
      Device *firstDev;
      firstDev = listdevices;
      int i = 0;
      while (listdevices->oui)
	{
	  printf ("%d. ", ++i);
	  addFlag (listdevices++, deviceFlag);
	}
      freeDevice (firstDev);
      deviceFlag->value = NULL;
      deviceFlag->name = NULL;
      free (deviceFlag->value);
      free (deviceFlag->name);
    }
  free (deviceFlag);
  return 0;
}

