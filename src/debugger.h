#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "sivm.h"
#include "parser.h"

typedef struct
{
    SIVM sivm;
    cmd_word *program;
} Debugger;

void debugger_new(Debugger *debug);
void debugger_start(Debugger *debug);

#endif /*DEBUGGER_H*/
