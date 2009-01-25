#include "mot.h"

bool checkModes(mot *m)
{
    if (m->codage.codeop == ADD)
    {
        switch(m->codage.mode)
        {
            case REGREG:
                return true;
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
    return false;
}
