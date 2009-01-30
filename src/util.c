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
		case 0:
			color = "[41m\e[30m";
			break;
		case 1:
			color = "[43m\e[30m";
			break;
		case 2:
			color = "[31m";
			break;
		case 5:
			color = "[34m";
			break;
		default:
			break;
	}

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
