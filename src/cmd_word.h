#ifndef CMD_WORD_H
#define CMD_WORD_H

#include "sivm.h"
#include "instructions.h"

/**Maximum length for a string representing a command word in assembly code.*/
#define MAX_INSTR_PRINT_SIZE 200

bool getModes(cmd_word *w, mode *destMode, mode *sourceMode);

char* disassemble(int length, const cmd_word words[]);
char* disassemble_no_formatting(int length, const cmd_word words[]);

#endif
