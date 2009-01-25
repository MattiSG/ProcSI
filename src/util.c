#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void quit(char *msg)
{
    fprintf(stderr, "[erreur] %s\n", msg);
    exit(1);
}
