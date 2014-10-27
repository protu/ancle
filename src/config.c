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

static int readConfig(char *filename, acs *data);
int envSetting(acs *data);
static void strip(char *string);
void freeACSData(acs *data);
static acs *serverdata;

/*
 * parse return 0 if ACS is not parsed and 1 if it is
 */

acs *setACS()
{
    if (serverdata == NULL)
        return 0;

    return serverdata;
}

int
parseConfig(char *userConfigFile)
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

    if (readConfig(userConfigFile, serverdata))
        ;
    else if (envSetting(serverdata))
        ;
    else if (readConfig(file, serverdata))
        ;
    else if (readConfig(GLOBAL_CONFIG, serverdata))
        ;
    else {
        fprintf(stderr, "Can't set ACS server\n");
        exit = 0;
    }

    if (!exit && verbose)
        printf("URL: %s\nuser: %s\npass: %s\n", serverdata->url,
               serverdata->user, serverdata->pass);

    free(file);

    return exit;
}

void
freeACS(acs *serverdata)
{
    freeACSData(serverdata);
    free(serverdata);
    serverdata = NULL;
}

int
envSetting(acs *serverdata)
{
    char *acsurl = getenv("ACS_URL");
    char *acsuser = getenv("ACS_USER");
    char *acspass = getenv("ACS_PASS");

	if(verbose)
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

static int
readConfig(char *file, acs * serverdata) {
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

	if(verbose)
	  printf("Reading file: %s\n", file);

    while (1) {
        if ((fgets(line, MAXLINE, cnf)) == NULL)
			break;
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

static void
strip(char * str)
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

void
freeACSData(acs *serverdata)
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
