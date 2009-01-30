#include <stdio.h>
#include <stdlib.h>

#include "util.h"

bool readLine(char *str, size_t length)
{
    // construit la chaîne de formatage de l'entrée
    size_t t_format = 81;
    char *format = calloc(t_format, sizeof(*format));
    snprintf(format, t_format, "%%%d[^\n]", (unsigned int) length);

    int ret = scanf(format, str);

    // vide le buffer d'entrée
    char c;
    while (((c = getchar()) != '\n') && c != EOF);

    return ret == 1;
}

void logm(char level, char *format, ...)
{
    va_list args;
    va_start(args, format);
	
	char *color;
	switch (level) {
		case LOG_STEP:
			color = "[32m";
			break;
		case LOG_FATAL_ERROR:
			color = "[41m\e[30m";
			break;
		case LOG_ERROR:
			color = "[31m";
			break;
		case LOG_WARNING:
			color = "[33m";
			break;
		case LOG_INFO:
			color = "[0m";
			break;
		case LOG_DEBUG:
			color = "[34m";
			break;
		default:
			break;
	}
	
	if (level == LOG_STEP)
		level = OUT_LOG_LEVEL; //This way, we make sure important steps are always displayed
	
	if (level <= OUT_LOG_LEVEL)
    {
		if (ANSI_OUTPUT) {
			fprintf(stdout, "\e");
			fprintf(stdout, color);
		}
        vfprintf(stdout, format, args);
        if (ANSI_OUTPUT) fprintf(stdout, "\e[0m\n");
		else fprintf(stdout, "\n");
    }
	else if (level <= ERR_LOG_LEVEL)
    {
		if (ANSI_OUTPUT) {
			fprintf(stderr, "\e");
			fprintf(stderr, color);
		}
        vfprintf(stderr, format, args);
        if (ANSI_OUTPUT) fprintf(stdout, "\e[0m\n");
		else fprintf(stderr, "\n");
    }
	
	if (level <= FATAL_LEVEL)
		exit(1);
		
    va_end(args);
}

bool superRecover(REG *val)
{
	printf("SuperRecover has your back!\nPlease modify the value that caused the invalid access (%d), or type nothing to continue with the fatal error: ", *val);
	int buffer;
	if (! scanf("%d", &buffer)) {
		printf("Well, we tried to save you...\n");
		return false;
	}
	printf("%d\n", buffer);
	*val = (REG) buffer;
	return true;
}