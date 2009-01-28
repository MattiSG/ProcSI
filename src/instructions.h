#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdbool.h>
#include "sivm.h"
#include "util.h"

/**Lists all available instructions.
 *The value of the enum elements are the opcodes for the given instruction.
 */
enum instructions
{
    LOAD = 0,
    STORE,
    ADD,
    SUB,
    JMP,
    JEQ,
    CALL,
    RET,
    PUSH,
    POP,
    HALT
};

/**@name	Adressing modes*/
//@{
/**Lists all available adressing modes.
 *TODO: Should be refactored to single-type adressing modes only.
 */
typedef enum
{
	REGREG = 0x0,
	REGIMM = 0x4,
	REGDIR = 0x8,
	REGIND = 0xC,
	DIRIMM = 0x5,
	DIRREG = 0x1,
	INDIMM = 0x6,
	INDREG = 0x2,
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
    bool (*function)(SIVM *sivm, REG *dest, const mot source);
    f_mode modes;
} Instr;

bool checkModes(mot *m);

Instr getInstruction(mot *m);

#endif