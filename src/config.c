/*
 * Functions for parsing configuration file and
 * environment variables
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "ancle.h"

#define MAXLINE 250

static int read(char *filename, acs *data);
static int envSetting(acs *data);
static void strip(char *string);
static void freeACSData(acs *data);

int parse(acs *serverdata, char *userConfigFile)
{
	char *file = NULL;
	char *home;
	int exit = 0;

	if ((serverdata = malloc(sizeof(acs))) == NULL)
	{
		fprintf(stderr, "Not enough memory\n");
		return 0;
	}

	serverdata->url = NULL;
	serverdata->user = NULL;
	serverdata->pass = NULL;

	file = NULL;
	home = getenv("HOME");
	const char *fileName = LOCAL_CONFIG;
	if ((file = calloc( sizeof(char), (strlen(home) + 1) * sizeof(char) + (strlen(fileName) + 1) * sizeof(char))) == NULL)
	{
		fprintf(stderr, "Not enough memory\n");
		return 0;
	}

	strcat(file, home);
	strcat(file, fileName);

	if (read(userConfigFile, serverdata))
		;
	else if (envSetting(serverdata))
		;
	else if (read(file, serverdata))
		;
	else if (read(GLOBAL_CONFIG, serverdata))
		;
	else {
		fprintf(stderr, "Can't set ACS server\n");
		exit = 1;
	}

	if (!exit)
		printf("URL: %s\nuser: %s\npass: %s\n", serverdata->url,
				serverdata->user, serverdata->pass);

	free(file);
	freeACS(serverdata);

	return exit;
}

void freeACS(acs *serverdata)
{
	freeACSData(serverdata);
	free(serverdata);
	serverdata = NULL;
}

static int envSetting(acs *serverdata)
{
	char *acsurl = getenv("ACS_URL");
	char *acsuser = getenv("ACS_USER");
	char *acspass = getenv("ACS_PASS");

	printf("Checking environment\n");

	if (acsurl && acsuser && acspass) {
		if ((serverdata->url = malloc((strlen(acsurl) + 1) * sizeof(char)))
				== NULL) {
			fprintf(stderr, "Not enough memory\n");
			freeACSData(serverdata);
			return 0;
		}
		if ((serverdata->user = malloc((strlen(acsuser) + 1) * sizeof(char)))
				== NULL) {
			fprintf(stderr, "Not enough memory\n");
			freeACSData(serverdata);
			return 0;
		}
		if ((serverdata->pass = malloc((strlen(acspass) + 1) * sizeof(char)))
				== NULL) {
			fprintf(stderr, "Not enough memory\n");
			freeACSData(serverdata);
			return 0;
		}

		strcpy(serverdata->url, acsurl);
		strcpy(serverdata->user, acsuser);
		strcpy(serverdata->pass, acspass);
	} else
		return 0;

	return 1;
}

static int read(char *file, acs * serverdata) {
	FILE *cnf = NULL;
	char *value;
	char line[MAXLINE];

	if(file == NULL)
		return 0;

	serverdata->url = NULL;
	serverdata->user = NULL;
	serverdata->pass = NULL;

	cnf = fopen(file, "r");

	if (cnf == NULL) {
		fprintf(stderr, "Can't open file %s . %s\n", file, strerror(errno));
		return 0;
	}
	printf("Reading file: %s\n", file);

	while (1) {
		fgets(line, MAXLINE, cnf);
		if (feof(cnf))
			break;
		if (strchr(line, '#') == line)
			;
		else if ((value = strchr(line, '=')) != NULL) {
			value[0] = '\0';
			strip(++value);
			strip(line);

			if (strstr(line, "url") == line) {
				if ((serverdata->url = malloc(
								(strlen(value) + 1) * sizeof(char))) == NULL) {
					fprintf(stderr, "Not enough memory\n");
					freeACSData(serverdata);
					return 0;
				}
				strcpy(serverdata->url, value);
			} else if (strstr(line, "user") == line) {
				if ((serverdata->user = malloc(
								(strlen(value) + 1) * sizeof(char))) == NULL) {
					fprintf(stderr, "Not enough memory\n");
					freeACSData(serverdata);
					return 0;
				}
				strcpy(serverdata->user, value);
			} else if (strstr(line, "pass") == line) {
				if ((serverdata->pass = malloc(
								(strlen(value) + 1) * sizeof(char))) == NULL) {
					fprintf(stderr, "Not enough memory\n");
					freeACSData(serverdata);
					return 0;
				}
				strcpy(serverdata->pass, value);
			}
		}
	}
	fclose(cnf);

	if (!(serverdata->url && serverdata->user && serverdata->pass)) {
		freeACSData(serverdata);
		return 0;
	}

	return 1;
}

static void strip(char * str)
{
	size_t len;
	int i;

	len = strlen(str) - 1;
	while (isspace(str[len]) && len > 0) {
		str[len] = '\0';
		len--;
	}

	len = strlen(str);
	i = 0;
	while (i < len && isspace(str[i])) {
		i++;
	}
	memmove(str, str + i, len + 1 - i);
}

static void freeACSData(acs *serverdata)
{
	if (serverdata->url)
		free(serverdata->url);
	if (serverdata->user)
		free(serverdata->user);
	if (serverdata->pass)
		free(serverdata->pass);

	serverdata->url = NULL;
	serverdata->user = NULL;
	serverdata->pass = NULL;
}
