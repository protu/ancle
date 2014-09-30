#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAXLINE 250
#define LOCAL_CONFIG "/.anclerc"
#define GLOBAL_CONFIG "/etc/anclerc"

int read(char *filename);
void strip(char *string);


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
	
	if (read(GLOBAL_CONFIG))
		;
	else if(read(file))
		;
	else
		fprintf(stderr, "Can't open file %s . %s\n", file, strerror(errno));

	free(file);
	return 1;
}

int read(char *file) {
	FILE *cnf = NULL;
	char *value;
	char line[MAXLINE];

	cnf = fopen(file, "r");

	if (cnf == NULL)
	{
		fprintf(stderr, "Can't open file %s . %s\n", file, strerror(errno));
		return 0;
	}

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
			printf("Token: %s\nValue: %s\n\n", line, value);
		}
	}
	fclose(cnf);
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
		

