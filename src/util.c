#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void quit(char *msg)
{
    fprintf(stderr, "[erreur] %s\n", msg);
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

void logm(char *msg, char level)
{
	if (level <= OUT_LOG_LEVEL)
	    fprintf(stdout, "%s\n", msg);
	else if (level <= ERR_LOG_LEVEL)
	    fprintf(stderr, "%s\n", msg);
}
