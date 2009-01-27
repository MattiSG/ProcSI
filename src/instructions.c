#include "instructions.h"

#include "util.h"

bool instr_load(SIVM *sivm, REG *dest, const mot source)
{
	*dest = source.brut;
    return true;
}

bool instr_store(SIVM *sivm, REG *dest, const mot source)
{
	*dest = source.brut;
    return true;
}

bool instr_add(SIVM *sivm, REG *dest, const mot source)
{
	*dest += source.brut;
    return true;
}

bool instr_sub(SIVM *sivm, REG *dest, const mot source)
{
	*dest -= source.brut;
    return true;
}

Instr instructions[] = {
	[LOAD] = {instr_load, FM_REGDIR | FM_REGIMM | FM_REGIND},
	[STORE] = {instr_store, FM_DIRIMM | FM_DIRREG | FM_INDIMM | FM_INDREG},
	[ADD] = {instr_add, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND},
	[SUB] = {instr_sub, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND}
};

bool checkModes(mot *m)
{
    return getInstruction(m).modes & (1 << m->codage.mode);
}

Instr getInstruction(mot *m)
{
	return instructions[m->codage.codeop];
}