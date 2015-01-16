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
 * @file ancle.c

 ** @brief Ancle - ACSLight NBI Command Line interface main function
 *
 * This file contain main class will only take command line input
 * paramteres and run function to parse configuration file.
 *
 */

#include "config.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ancle.h"

/** @var int verbose
 * Verbose switch. If set to 1, program will output
 * debug informations to the standard output
 */

int verbose = 0;

/** @var int yes
 * Assume yes switch. If set to 1, program will assume
 * yes on all promits and skip them.
 */
int yes = 0;

/**
 * Print help message to the console
 */

void
print_help ()
{
  char *help =
      " \
ancle - ACSLite command line interface. \n\n \
ancle will use ACSLite's NBI for communication and can be used \
from remote host to retrieve information from it\n\n \
Usage: ancle command [options]\n \
Commands:\n \
-h --help\tprint this help\n \
-V --version\tprint version and exit\n \
-R --register\tregister device\n \
-S --search\tsearch device\n \
-D --delete\tdelete device\n \
-F --flag\tflag operations\n \
Options:\n \
-o --oui=oui\tdevice's object unique identifier\n \
-p --product-class=product class\tdevices's product class\n \
-s --serial-number=serial number\tdevices's serial number\n \
-d --description=description\t\tdevice's description.\n \
-n --name\t\tflag or attribute name.\n \
-v --value\t\tflag or attribute value.\n \
Other:\n \
-f --config-file\tuser alternative configuration file\n \
-y --yes\t\tasume yes on all prompts\n \
--verbose\tbe verbose\n";

  puts (help);
}

/**
 * Just to get default input parameters and fire up 
 * required action for devices
 */

int
main (int argc, char **argv)
{
  int c;
  char action = NULL;
  char *userConfigFile = NULL;
  acs *serverdata = NULL;

  Device *dev;
  if ((dev = malloc (sizeof(Device))) == NULL)
    {
      fprintf (stderr, "Not enough memory\n");
      return 1;
    }

  flag *devFlag;
  if ((devFlag = malloc (sizeof(flag))) == NULL)
    {
      fprintf (stderr, "Not enough memory\n");
      return 1;
    }

  dev->productclass = NULL;
  dev->serialnumber = NULL;
  dev->oui = NULL;
  dev->description = NULL;

  static struct option long_options[] =
    {
      { "version", no_argument, 0, 'V' },
      { "help", no_argument, 0, 'h' },
      { "register", no_argument, 0, 'R' },
      { "delete", no_argument, 0, 'D' },
      { "search", no_argument, 0, 'S' },
      { "product-class", required_argument, 0, 'p' },
      { "serial-number", required_argument, 0, 's' },
      { "oui", required_argument, 0, 'o' },
      { "description", required_argument, 0, 'd' },
      { "config-file", required_argument, 0, 'f' },
      { "yes", no_argument, 0, 'y' },
      { "verbose", no_argument, &verbose, 1 },
      { "flag", no_argument, 0, 'F' },
      { "name", required_argument, 0, 'n' },
      { "value", required_argument, 0, 'v' },
      { 0, 0, 0, 0 } };

  int option_index = 0;

  if (argc < 2)
    {
      print_help ();
      return 0;
    }

  while (1)
    {
      c = getopt_long (argc, argv, "VhRDSFn:v:yo:p:s:d:f:", long_options,
		       &option_index);
      if (c == -1)
	break;

      switch (c)
	{
	case 'V':
	  printf ("ancle 0.0.2a\n");
	  return 0;
	case 'h':
	  print_help ();
	  return 0;
	case 'R':
	  action = 'R';
	  break;
	case 'D':
	  action = 'D';
	  break;
	case 'S':
	  action = 'S';
	  break;
	case 'F':
	  action = 'F';
	  break;
	case 'f':
	  userConfigFile = optarg;
	  break;
	case 'p':
	  dev->productclass = optarg;
	  break;
	case 's':
	  dev->serialnumber = optarg;
	  break;
	case 'd':
	  dev->description = optarg;
	  break;
	case 'o':
	  dev->oui = optarg;
	  break;
	case 'y':
	  yes = 1;
	  break;
	case 'n':
	  devFlag->name = optarg;
	  break;
	case 'v':
	  devFlag->value = optarg;
	  break;
	case ':':
	case '?':
	  print_help ();
	  return 1;
	}
    }

  /*
   * Parse ACS sever connection data
   */

  if (parseConfig (userConfigFile))
    {
      fprintf (stderr, "No ACS server defined\n");
      return 1;
    }

  if (action)
    printf ("Using ACS URL: %s\n\n", setACS ()->url);

  /*
   * Check what action is defined and issue it
   */

  switch (action)
    {
    case 'R':
      if (dev->productclass == NULL || dev->serialnumber == NULL
	  || dev->oui == NULL)
	{
	  printf ("OUI, Product Class and Serial Number must be specified\n");
	  printf ("OUI: %s\n", dev->oui);
	  printf ("Product class: %s\n", dev->productclass);
	  printf ("Serial Number: %s\n", dev->serialnumber);
	  printf ("Description: %s\n", dev->description);
	  return 0;
	}
      else
	{
	  regdevice (dev);
	}
      break;
    case 'D':
      if (dev->productclass == NULL && dev->serialnumber == NULL
	  && dev->oui == NULL && dev->description == NULL)
	{
	  printf (
	      "OUI, Product Class, Description or Serial Number must be specified\n");
	  printf ("OUI: %s\n", dev->oui);
	  printf ("Product class: %s\n", dev->productclass);
	  printf ("Serial Number: %s\n", dev->serialnumber);
	  printf ("Description: %s\n", dev->description);
	  return 0;
	}
      else
	{
	  deldevices (dev);
	}
      break;
    case 'S':
      if (dev->productclass == NULL && dev->serialnumber == NULL
	  && dev->oui == NULL && dev->description == NULL)
	{
	  printf (
	      "OUI, Product Class, Description or Serial Number must be specified\n");
	  printf ("OUI: %s\n", dev->oui);
	  printf ("Product class: %s\n", dev->productclass);
	  printf ("Serial Number: %s\n", dev->serialnumber);
	  printf ("Description: %s\n", dev->description);
	  return 0;
	}
      else
	{
	  getdevices (dev);
	}
      break;
    case 'F':
      if (dev->productclass == NULL && dev->serialnumber == NULL
	  && dev->oui == NULL && dev->description == NULL)
	{
	  printf (
	      "OUI, Product Class, Description or Serial Number must be specified\n");
	  printf ("OUI: %s\n", dev->oui);
	  printf ("Product class: %s\n", dev->productclass);
	  printf ("Serial Number: %s\n", dev->serialnumber);
	  printf ("Description: %s\n", dev->description);
	  return 0;
	}
      else if (devFlag->name == NULL)
	{
	  printf ("Flag name must be specified\n");
	  return 0;
	}
      else
	{
	  flagdevices (dev, devFlag);
	}
      break;
    default:
      puts ("No action requested!");
      print_help ();
    }

  if ((serverdata = setACS ()))
    freeACS (serverdata);

  free (dev);
  dev = NULL;

  return 0;
}
