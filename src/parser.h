#ifndef PARSER_H
#define PARSER_H

#define _POSIX_SOURCE
#include <stdlib.h>
#include <string.h>

#include "sivm.h"
#include "util.h"
#include "instructions.h"

/**Parse a procsi assembly source, and loads it in a virtual machine
 *Since mem is allocated in this function, it's up to you to free it when not needed anymore
 *@see      svim_parse_file
 *@param    memsize output int pointer to the code's size in memory
 *@param    mem     output cmd_word array of size memsize, containing assembled code
 *@param    str     the source code of the program to assemble
 *@returns	false if any error occured (stderr is written in consequence with perror), true if the source is assembled and loaded corretly.
 */
bool sivm_parse(int* memsize, cmd_word *mem[], char *str);

/**Parse a procsi assembly file, and loads it in a virtual machine
 *Since mem is allocated in this function, it's up to you to free it when not needed anymore
 *@see      svim_parse
 *@param    memsize output int pointer to the code's size in memory
 *@param    mem     output cmd_word array of size memsize, containing assembled code
 *@param    str     the filename of the source code
 *@returns	false if any error occured (stderr is written in consequence with perror), true if the file is assembled and loaded corretly.
 */
bool sivm_parse_file(int* memsize, cmd_word *mem[], char *file);

#endif /* PARSER_H */
