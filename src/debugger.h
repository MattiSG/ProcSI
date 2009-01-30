#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "sivm.h"
#include "parser.h"

/**
 * \struct Debugger
 * \brief  Structure for debugging
 */
typedef struct
{
    SIVM sivm;         /*!< pointer to virtual machine */
    cmd_word *program; /*!< pointer to array of commands */
	size_t programSize; /*!< size of the loaded program, in number of cmd_word*/
} Debugger;

/**
 * \fn    void debugger_new(Debugger *debug)
 * \brief Initialize a Debugger structure
 */
void debugger_new(Debugger *debug);

/**
 * \fn    void debugger_start(Debugger *debug)
 * \brief Start the debugger
 */
void debugger_start(Debugger *debug);

#endif /*DEBUGGER_H*/
