#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXLINE 250

void read();
void strip(char *);

int main()
{
	read();
	return 0;
}

void read() {
	FILE *cnf = NULL;
	char line[MAXLINE];
	cnf = fopen("anclerc", "r");
	char  *value;

	if (cnf == NULL)
	{
		fprintf(stderr, "Can't open file\n");
		exit(0);
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
		

