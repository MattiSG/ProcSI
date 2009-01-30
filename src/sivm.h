#ifndef SIVM_H
#define SIVM_H

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

typedef uint16_t REG;

/**@name	SIVM parameters*/
//@{
/**Number of registers in an SIVM.*/
#define NREGS 8
/**First register to be reserved for return values.
 *The registers in this reserved range won't be updated by a call to RET, and not saved by a call to CALL.
 *<strong>NOTE</strong>: the value is a register number, not an index (ie. starts at 1, not 0)
 *@see	PARAM_REGS_END
 */
#define PARAM_REGS_START 1
/**Last register to be reserved for return values.
 *The registers in this reserved range won't be updated by a call to RET, and not saved by a call to CALL.
 *<strong>NOTE</strong>: the value is a register number, not an index (ie. starts at 1, not 0)
 *@see	PARAM_REGS_START
 */
#define PARAM_REGS_END 4
/**Size of an SIVM's memory*/
#define MEMSIZE 128
/**PC index at SIVM startup*/
#define PC_START 0
/**SR index at SIVM startup*/
#define SR_START 0
/**SP index at SIVM startup*/
#define SP_START MEMSIZE-1
/**SP incrementation
 *Set it to (+)1 to go through ascending adresses, -1 to go through descending adresses.
 *Standard operation is found by setting SP_START at MEMSIZE and this variable to -1 (descending adresses).
 */
#define SP_INCR -1
//@}

/**@name	Status registers conventions
 *Defines the numerical values for the SP, SR and PC registers used internally.
 *<strong>WARNING</strong>: do not set these to anything between 0 and NREGS!
 */
//@{
#define PC 100
#define SP 101
#define SR 102
//@}


/**@name	Command words definition
 *Defines the order of components in a command word, and the inner form of a command word too.
 */
//@{
#define CMD_WORD_OPCODE_INDEX 0
#define CMD_WORD_MODE_INDEX 1
#define CMD_WORD_SOURCE_INDEX 2
#define CMD_WORD_DEST_INDEX 3

/*This definition isn't in cmd_word.h because of unresolvable recursive includes.*/
typedef union
{
	REG brut;
	struct
	{
		unsigned codeop : 6;
		unsigned mode   : 4;
		unsigned source : 3;
		unsigned dest   : 3;
	} codage;
} cmd_word;
//@}

typedef struct {
    REG pc;
    REG sp;
    REG sr;
    REG reg[NREGS];
	cmd_word mem[MEMSIZE];
} SIVM;

/**
 * \brief Initializes a new ProcSI virtual machine
 * \author Me
 */
void sivm_new(SIVM *sivm);
bool sivm_load(SIVM *sivm, int memsize, cmd_word mem[memsize]);
bool sivm_step(SIVM *sivm);

void sivm_status(SIVM *sivm);
bool sivm_print_register(SIVM *sivm, unsigned int reg);
bool sivm_print_memory(SIVM *sivm, unsigned int mem);

bool checkMemoryAccess(REG index);
bool checkRegisterAccess(unsigned index);

char* sivm_get_instruction_string(SIVM *sivm);

#endif /*SIVM_H*/
