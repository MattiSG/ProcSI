#ifndef PARSER_H
#define PARSER_H

#define _POSIX_SOURCE
#include <stdlib.h>
#include <string.h>

#include "sivm.h"
#include "util.h"
#include "instructions.h"

/**Parse a procsi assembly source, and loads it in a virtual machine
 *@see      svim_parse_file
 *@param    sivm    a pointer to SIVM structure
 *@param    str     the source code of the program to assemble
 *@returns	false if any error occured (stderr is written in consequence with perror), true if the source is assembled and loaded corretly.
 */
bool sivm_parse(int memsize, mot mem[memsize], char *str);

/**Parse a procsi assembly file, and loads it in a virtual machine
 *@see      svim_parse
 *@param    sivm a pointer to SIVM structure
 *@param    str     the filename of the source code
 *@returns	false if any error occured (stderr is written in consequence with perror), true if the file is assembled and loaded corretly.
 */
bool sivm_parse_file(int memsize, mot mem[memsize], char *file);

#endif /* PARSER_H */
