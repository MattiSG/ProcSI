#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdbool.h>

#include "sivm.h"
#include "parser.h"

/**
 * \struct Debugger
 * \brief  Structure for debugging
 */
typedef struct
{
    SIVM sivm;          /*!< pointer to virtual machine */
    char *filename;     /*!< filename of the binary program */
    ParserResult presult;
    bool is_source;
} Debugger;

/**
 * \fn    void debugger_new(Debugger *debug)
 * \brief Initialize a Debugger structure
 */
void debugger_new(Debugger *debug, char *filename, bool isSource);

/**
 * \fn    void debugger_start(Debugger *debug)
 * \brief Start the debugger
 */
void debugger_start(Debugger *debug);

#endif /*DEBUGGER_H*/
