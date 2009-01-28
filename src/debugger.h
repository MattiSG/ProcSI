#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "sivm.h"

typedef struct
{
    SIVM sivm;
} Debugger;

void debugger_new(Debugger *debug);
void debugger_start(Debugger *debug);

#endif /*DEBUGGER_H*/
