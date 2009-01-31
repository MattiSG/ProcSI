#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdbool.h>

#include "sivm.h"
#include "parser.h"

/**
 * @struct Debugger
 * @brief  Structure for debugging
 */
typedef struct
{
    SIVM sivm;              /*!< pointer to virtual machine */
    char *filename;         /*!< filename of the binary program */
    ParserResult presult;   /*!< parsing result */
    bool is_source;         /*!< filename is a source or a binary file */
} Debugger;

/**
 * @brief Initialize a Debugger structure
 * @param debug     pointer to debugger structure
 * @param filename  source or binary file
 * @param is_source type of file
 */
void debugger_new(Debugger *debug, char *filename, bool is_source);

/**
 * @brief Start the debugger
 * @param debug pointer to debugger structure
 */
void debugger_start(Debugger *debug);

#endif /*DEBUGGER_H*/
