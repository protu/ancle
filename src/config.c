#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAXLINE 250
#define USER_CONFIG "anclerc"
#define LOCAL_CONFIG "/.anclerc"
#define GLOBAL_CONFIG "/etc/anclerc"

int read(char *filename);
void strip(char *string);
int envSetting();


int main()
{
	
	char *file = NULL;
	char *home;

	file = NULL;
	home = getenv("HOME");
	const char *fileName = LOCAL_CONFIG;
	file = malloc((strlen(home)+1)*sizeof(char) + (strlen(fileName)+1)*sizeof(char));
	strcat(file, home);
	strcat(file, fileName);
	
	if (read(USER_CONFIG)) 
		;
	else if (envSetting()) 
		;
	else if(read(file))
		;
	else if (read(GLOBAL_CONFIG))
		;
	else
		fprintf(stderr, "Can't set ACS server\n");

	free(file);
	return 1;
}

int envSetting()
{
	char *acsurl = getenv("ACS_URL");
	char *acsuser = getenv("ACS_USER");
	char *acspass = getenv("ACS_PASS");

	printf("Checking environment\n");

	if (acsurl && acsuser && acspass)
	{
		printf("URL: %s\nuser: %s\npass: %s\n", acsurl, acsuser, acspass);
	}
	else
		return 0;

	return 1;
}


int read(char *file) {
	FILE *cnf = NULL;
	char *value, *acsurl, *acsuser, *acspass;
	char line[MAXLINE];

	acsurl = NULL;
	acsuser = NULL;
	acspass = NULL;

	cnf = fopen(file, "r");

	if (cnf == NULL)
	{
		fprintf(stderr, "Can't open file %s . %s\n", file, strerror(errno));
		return 0;
	}
	printf("Reading file: %s\n", file);

	while (1)
	{
		fgets(line, MAXLINE, cnf);
		if(feof(cnf))
			break;
		if (strchr(line, '#') == line)
			;
		else if ((value = strchr(line, '=')) != NULL)
		{
			value[0] = '\0';
			strip(++value);
			strip(line);
//			printf("Token: %s\nValue: %s\n\n", line, value);

			if (strstr(line, "url") == line)
			{
				if ((acsurl = malloc((strlen(value)+1)*sizeof(char))) == NULL)
				{
					fprintf(stderr, "Not enough memory\n");
					return 0;
				}
				strcpy(acsurl, value);
			}
			else if (strstr(line, "user") == line)
			{
				if ((acsuser = malloc((strlen(value)+1)*sizeof(char))) == NULL)
				{
					fprintf(stderr, "Not enough memory\n");
					return 0;
				}
				strcpy(acsuser, value);
			}
			else if (strstr(line, "pass") == line)
			{
				if ((acspass= malloc((strlen(value)+1)*sizeof(char))) == NULL)
				{
					fprintf(stderr, "Not enough memory\n");
					return 0;
				}
				strcpy(acspass, value);
			}
		}
	}
	fclose(cnf);

	if (acsurl && acsuser && acspass)
	{
		printf("URL: %s\nuser: %s\npass: %s\n", acsurl, acsuser, acspass);
		free(acsurl);
		free(acsuser);
		free(acspass);
		acsurl = NULL;
		acsuser = NULL;
		acspass = NULL;
	}
	else
	{
		if (acsurl)
			free(acsurl);
		if (acsuser)
			free(acsuser);
		if (acspass)
			free(acspass);

		return 0;
	}

	return 1;
}

void strip(char * str)
{
	size_t len;
	int i;

	len = strlen(str) - 1;
	while (isspace(str[len]) && len > 0)
	{
		str[len] = '\0';
		len--;
	}

	len = strlen(str);
	i = 0;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	memmove(str, str+i, len+1-i);
}
		

