#ifndef ANCLE_DEF
#define ANCLE_DEF

#define ACS_NBI_URL "http://l01acslab.ot.hr/nbi.php"
#define ACS_NBI_USERPWD  "nbiuser:nbipass"

/**
 * Storage for retreived SOAP request
**/

struct MemoryStruct
{
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
typedef struct DeviceStruct
{
	char *oui;
	char *productclass;
	char *serialnumber;
	char *description;
} Device;

/**
 * Define Pointer for Device structure
**/

typedef Device *DevicePtr;



int getdevices(char *productClass, char *serialNumber);
int callCurl(char *request, struct MemoryStruct *response);
char *soapreq(DevicePtr dev);
int totalRecords(char * response);
int devicesFound(char *response, int total);

#endif
