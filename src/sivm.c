#include "sivm.h"

#include "instructions.h"

bool sivm_new(SIVM *sivm)
{
    sivm->pc = 0;
    sivm->sp = 0;
    sivm->sr = 0;

    for (unsigned int i = 0; i < NREGS; i++)
        sivm->reg[i] = 0;
    for (unsigned int i = 0; i < MEMSIZE; i++)
        sivm->mem[i].brut = 0;

    return true;
}

bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize])
{
    for (unsigned int i = 0; i < memsize; i++)
        sivm->mem[i] = mem[i];

    return true;
}

bool sivm_step(SIVM *sivm)
{
    mot *m = &sivm->mem[sivm->pc];

    /* stop the vm */
    if (m->codage.codeop == HALT)
        return false;

    if (!checkModes(m))
        return false;

    return sivm_exec(sivm, m);
}

void sivm_status(SIVM *sivm)
{
    printf("==> Status <==\n");
    for (unsigned int i = 0; i < NREGS; ++i)
        printf("\tREG[%d] = %d\n", i, sivm->reg[i]);
}

bool sivm_exec(SIVM *sivm, mot *word)
{
	if (! checkModes(word)) {
		quit("Illegal modes");
		return false;
	}
	
	sivm->pc++;
	
	Instr instr = getInstruction(word);
	REG dest;
	mot source;
	
	return instr.function(sivm, dest, source);
}