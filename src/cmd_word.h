#ifndef CMD_WORD_H
#define CMD_WORD_H

#include "sivm.h"
#include "instructions.h"

/**Maximum length for a string representing a command word in assembly code.*/
#define MAX_INSTR_PRINT_SIZE 15

bool getModes(cmd_word *word, mode *destMode, mode *sourceMode);

#endif
