/**
 * @file ancle.h
 * @brief main header file for ANCLE
 *
 * File contains funcion, globals, definitions  and macros for Ancle program
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
 * member of the structure exept oui which is 6
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
 * Globals
 **/

extern int verbose;
extern int yes;

/**
 * Global functions
 **/

/* getdevices */
int getdevices(Device *device);
int deldevices(Device *device);
int regdevice(Device *device);

/* callcurl */

int callCurl(char *request, struct MemoryStruct *response);

/* parseResponse */

char *soapSearch(DevicePtr dev);
char *soapRegister(DevicePtr dev);
char *soapDelete(DevicePtr dev);
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
