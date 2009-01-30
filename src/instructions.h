#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdbool.h>
#include "sivm.h"
#include "util.h"

/**Lists all available instructions.
 *The value of the enum elements are the opcodes for the given instruction.
 *Some values are given according to the A.A. courses documents, others are given arbitrarily.
 */
enum instructions
{
    LOAD =	0x8, //ref: TD2 A.A.
    STORE =	0x9, //ref: TD2 A.A.
    ADD =	0x0, //ref: TD2 A.A.
    SUB =	0x1, //ref: TD2 A.A.
    JMP =	0x2, //no ref from now on
    JEQ =	0x3,
    CALL =	0x4,
    RET =	0x5,
    PUSH =	0x6,
    POP =	0x7,
	MOV =	0xA,
    HALT =	0xF
};

/**@name	Adressing modes*/
//@{
/**Lists all available adressing modes.
 *The reference for the values is the document "Description de PROCSI" that was given to us in the A.A. courses.
 *We don't use single-type adressing modes only because it's impossible to achieve storing 5x5 possible combinations in 4 bits.
 *The single-type adressing modes (REGISTER, IMMEDIATE, DIRECT, INDIRECT) are not for public use.
 *@see	sivm.c#sivm_exec
 */
typedef enum
{
	REGREG = 0x0, // 0b0000
	REGIMM = 0x4, // 0b0100
	REGDIR = 0x8, // 0b1000
	REGIND = 0xC, // 0b1100
	DIRIMM = 0x5, // 0b0100
	DIRREG = 0x1, // 0b0001
	INDIMM = 0x6, // 0b0110
	INDREG = 0x2, // 0b0010
	REGISTER,
	IMMEDIATE,
	DIRECT,
	INDIRECT
} mode;

/**Shifted adressing modes.
 *TODO: move to the implementation file
 *TODO: explain clearly why this is done
 */
typedef enum
{
	FM_REGREG = 1 << REGREG,
	FM_REGIMM = 1 << REGIMM,
	FM_REGDIR = 1 << REGDIR,
	FM_REGIND = 1 << REGIND,
	FM_DIRIMM = 1 << DIRIMM,
	FM_DIRREG = 1 << DIRREG,
	FM_INDIMM = 1 << INDIMM,
	FM_INDREG = 1 << INDREG
} f_mode;
//@}


typedef struct {
    bool (*function)(SIVM *sivm, REG *dest, const cmd_word source);
    f_mode modes;
	char* name;
} Instr;

bool checkModes(const cmd_word m);

Instr getInstruction(const cmd_word m);

#endif
