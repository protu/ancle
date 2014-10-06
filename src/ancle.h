#ifndef ANCLE_DEF
#define ANCLE_DEF

#define ACS_NBI_URL "http://l01acslab.ot.hr/nbi.php"
#define ACS_NBI_USERPWD  "nbiuser:nbipass"
#define LOCAL_CONFIG "/.anclerc"
#define GLOBAL_CONFIG "/etc/anclerc"

extern int verbose;

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

int getdevices(Device *device);
int deldevices(Device *device);
int regdevice(Device *device);
int callCurl(char *request, struct MemoryStruct *response);
char *soapSearch(DevicePtr dev);
char *soapRegister(DevicePtr dev);
char *soapDelete(DevicePtr dev);
int totalRecords(char * response);
Device *devicesFound(char *response, int total);
void freeDevice(Device *deviceList);
void printDevice(Device *deviceList);
int createResult(char * response);
void freeACS(acs *serverdata);
int parse(acs *serverdata, char *userConfigFile);

#endif
