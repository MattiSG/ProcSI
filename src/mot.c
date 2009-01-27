#include "mot.h"

bool instr_load(SIVM *sivm, mot *m)
{
    switch (m->codage.mode)
    {
    case REGDIR:
        sivm->reg[m->codage.dest] = m->codage.source;
        break;
    case REGIMM:
        break;
    case REGIND:
        break;
    }
    return true;
}

bool instr_store(SIVM *sivm, mot *m)
{
    return false;
}

bool instr_add(SIVM *sivm, mot *m)
{
    switch (m->codage.mode)
    {
    case REGREG:
        sivm->reg[m->codage.dest] += sivm->reg[m->codage.source];
        break;
    case REGIMM:
        sivm->reg[m->codage.dest] += m->codage.source;
        break;
    }
    return true;
}

Instr instrs[] = {
    [LOAD] = {instr_load, FM_REGDIR | FM_REGIMM | FM_REGIND},
    [STORE] = {instr_store, FM_DIRIMM | FM_DIRREG | FM_INDIMM | INDREG},
    [ADD] = {instr_add, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND}
};

bool checkModes(mot *m)
{
    return instrs[m->codage.codeop].modes & (1 << m->codage.mode);
}

bool sivm_new(SIVM *sivm)
{
    unsigned int i;

    sivm->pc = 0;
    sivm->sp = 0;
    sivm->sr = 0;

    for (i = 0; i < NREGS; i++)
        sivm->reg[i] = 0;
    for (i = 0; i < MEMSIZE; i++)
        sivm->mem[i].brut = 0;

    return true;
}

bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize])
{
    unsigned int i;
    for (i = 0; i < memsize; i++)
        sivm->mem[i] = mem[i];

    return true;
}

bool sivm_instr(SIVM *sivm, mot *m)
{
    sivm->pc++;
    return instrs[m->codage.codeop].function(sivm, m);
}

bool sivm_step(SIVM *sivm)
{
    mot *m = &sivm->mem[sivm->pc];

    /* stop the vm */
    if (m->codage.codeop == HALT)
        return false;

    if (!checkModes(m))
        return false;

    return sivm_instr(sivm, m);
}

void sivm_status(SIVM *sivm)
{
    printf("==> Status <==\n");
    unsigned int i;
    for (i = 0; i < NREGS; ++i)
        printf("\tREG[%d] = %d\n", i, sivm->reg[i]);
}
