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
 * @file ancle.h
 * @brief main header file for ANCLE
 *
 * File contains function, globals, definitions  and macros for Ancle program
 */

#ifndef ANCLE_DEF
#define ANCLE_DEF
#include <libxml/parser.h>

#define ACS_NBI_URL "http://l01acslab.ot.hr/nbi.php"
#define ACS_NBI_USERPWD "nbiuser:nbipass"

/**
 * define user's configuration file
 */
#define LOCAL_CONFIG "/.anclerc"
/**
 * define global configuration file
 */
#define GLOBAL_CONFIG "/etc/anclerc"


/**
 * Storage for retreived SOAP request
 **/

struct MemoryStruct {
    char *memory;
    size_t size;
};

/**
 * Basic device structure
 *
 * Maximum string size (without null) is 64 bytes for every
 * member of the structure except oui which is 6
 *
 **/
typedef struct DeviceStruct {
    char *oui;
    char *productclass;
    char *serialnumber;
    char *description;
} Device;

/**
 * Define Pointer for Device structure
 **/
typedef Device *DevicePtr;

/**
 * ACS server data
 **/
typedef struct acsdata {
    char *url;
    char *user;
    char *pass;
} acs;

/**
 * Flag structure
 **/
typedef struct flagStruct {
	char *name;
	char *value;
} flag;

/**
 * Define Pointer for Flag structure
 **/
typedef flag *flagPtr;

/**
 * Globals
 **/

extern int verbose;
extern int yes;

/**
 * Global functions
 **/

/* getdevices */
int getdevices(Device *device, flag *deviceFlag);
int deldevices(Device *device);
int regdevice(Device *device);
int flagdevices(Device *device, flag *deviceFlag);

/* callcurl */

int callCurl(char *request, struct MemoryStruct *response);

/* parseResponse */

char *soapSearch(DevicePtr dev, flagPtr flp);
char *soapRegister(DevicePtr dev);
char *soapDelete(DevicePtr dev);
char *soapAddFlag (DevicePtr dev, flagPtr flp);
int totalRecords(char * response);
Device *devicesFound(char *response, int total);
void freeDevice(Device *deviceList);
void printDevice(Device *deviceList);
xmlChar *createResult(char * response);

/* config */

void freeACS(acs *serverdata);
int parseConfig(char *userConfigFile);
acs *setACS();

#endif
