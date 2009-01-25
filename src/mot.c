#include "mot.h"

bool checkModesLOAD(enum mode m)
{
    switch(m)
    {
        case REGDIR:
            return true;
        case REGIMM:
            return true;
        case REGIND:
            return true;
        default:
            return false;
    }
}

bool checkModesSTORE(enum mode m)
{
    switch(m)
    {
        case DIRIMM:
            return true;
        case DIRREG:
            return true;
        case INDIMM:
            return true;
        case INDREG:
            return true;
        default:
            return false;
    }
}

bool checkModesADD(enum mode m)
{
    switch(m)
    {
        case REGREG:
            return true;
        case REGIMM:
            return true;
        case REGDIR:
            return true;
        case REGIND:
            return true;
        default:
            return false;
    }
}

bool checkModesSUB(enum mode m)
{
    switch(m)
    {
        case REGREG:
            return true;
        case REGIMM:
            return true;
        case REGDIR:
            return true;
        case REGIND:
            return true;
        default:
            return false;
    }
}

bool checkModesJMP(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesJEQ(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesCALL(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesRET(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesPUSH(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesPOP(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModesHALT(enum mode m)
{
    switch(m)
    {
        default:
            return false;
    }
}

bool checkModes(mot *m)
{
    if (m->codage.codeop == LOAD)
        return checkModesLOAD(m->codage.mode);
    if (m->codage.codeop == STORE)
        return checkModesSTORE(m->codage.mode);
    if (m->codage.codeop == ADD)
        return checkModesADD(m->codage.mode);
    if (m->codage.codeop == SUB)
        return checkModesSUB(m->codage.mode);
    if (m->codage.codeop == JMP)
        return checkModesJMP(m->codage.mode);
    if (m->codage.codeop == JEQ)
        return checkModesJEQ(m->codage.mode);
    if (m->codage.codeop == CALL)
        return checkModesCALL(m->codage.mode);
    if (m->codage.codeop == RET)
        return checkModesRET(m->codage.mode);
    if (m->codage.codeop == PUSH)
        return checkModesPUSH(m->codage.mode);
    if (m->codage.codeop == POP)
        return checkModesPOP(m->codage.mode);
    if (m->codage.codeop == HALT)
        return checkModesHALT(m->codage.mode);

    return false;
}
