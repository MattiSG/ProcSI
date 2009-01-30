#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void quit(char *format, ...)
{
    char errformat[256];
    sprintf(errformat, "[error] %s\n", format);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, errformat, args);
    va_end(args);
    exit(1);
}

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

	if (level <= OUT_LOG_LEVEL)
    {
        vfprintf(stdout, format, args);
        fprintf(stdout, "\n");
    }
	else if (level <= ERR_LOG_LEVEL)
    {
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
    }

    va_end(args);
}
