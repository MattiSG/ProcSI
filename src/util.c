#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void quit(char *msg)
{
    fprintf(stderr, "[erreur] %s\n", msg);
    exit(1);
}

void logm(char *msg, char level)
{
	if (level >= OUT_LOG_LEVEL)
	    fprintf(stdout, "%s\n", msg);
	else if (level >= ERR_LOG_LEVEL)
	    fprintf(stderr, "%s\n", msg);
}
