#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ancle.h"

void print_help()
{
	char *help = " \
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
Options:\n \
-c --count=n\tnumber of devices to add or list\n \
-o --oui=oui\tdevice's object unique identifier\n \
-p --product-class=product class\tdevices's product class\n \
-s --serial-number=serial number\tdevices's serial number\n \
-d --description=description\t\tdevice's description. It is possible to use SQL wildcard %\n ";

	puts(help);
}

int main (int argc, char **argv)
{
	
	int c;
	char *productclass = NULL;
	char *serialnumber = NULL;
	char *oui = NULL;
	char *description = NULL;
	char action = NULL;


	static struct option long_options[] =
	{
		{"version", no_argument, 0, 'V'},
		{"help", no_argument, 0, 'h'},
		{"register", no_argument, 0, 'R'},
		{"delete", no_argument, 0, 'D'},
		{"search", no_argument, 0, 'S'},
		{"count", required_argument, 0, 'c'},
		{"product-class", required_argument, 0, 'p'},
		{"serial-number", required_argument, 0, 'n'},
		{"oui", required_argument, 0, 'o'},
		{"description", required_argument, 0, 'd'},
		{0, 0, 0, 0}
	};

	int option_index = 0;

	if(argc < 2)
		print_help();

	while(1)
	{
		c = getopt_long(argc, argv, "VhRDSc:o:p:n:", long_options, &option_index);
		if (c == -1)
			break;

		switch(c)
		{
			case 'V':
				printf ("ancle 0.0.1a\n");
				return 0;
			case 'h':
			    print_help();
		        break;
			case 'R':
				action = 'R';
				break;
			case 'D':
				action = 'D';
				break;
			case 'S':
				action = 'S';
				break;
			case 'p':
				productclass = optarg;
				break;
			case 'n':
				serialnumber = optarg;
				break;
			case 'd':
				description = optarg;
				break;
			case 'o':
				oui = optarg;
				break;
			case ':':
			case '?':
				print_help();
				break;
		}
	}

	switch(action)
	{
		case 'R':
			printf("Register is not implemented.\n");
			break;
		case 'D':
			printf("Delete is not implemented.\n");
			break;
		case 'S':
			if (productclass == NULL && serialnumber == NULL && oui == NULL && description == NULL)
			{
				printf("OUI, Product Class, Description or Serial Number must be specified\n");
				printf("OUI: %s\n", oui);
				printf("Product class: %s\n", productclass);
				printf("Serial Number: %s\n", serialnumber);
				printf("Description: %s\n", description);
				return 0;
			}
			else
			{
				getdevices(productclass, serialnumber);
			}
			break;
	}

	return 0;
}
