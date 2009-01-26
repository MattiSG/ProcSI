/**
 *
 *  Questions :
 *  1) Quelles options de compilations doit-on utiliser (std=c99, Wall, Werror, ansi) ?
 * TODO
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "mot.h"
#include "util.h"



int main(int argc, char *argv[])
{
    SIVM sivm;
    
    mot prgtest[] = {
        { .codage = { ADD, REGREG, 3, 4 }},
        { .codage = { ADD, REGREG, 4, 1 }}
    };

    sivm_new(&sivm);
    sivm_load(&sivm, sizeof(prgtest) / sizeof(prgtest[0]), prgtest);
    sivm.reg[3] = 1;
    sivm.reg[4] = 2;
    sivm_status(&sivm);
    while (sivm_step(&sivm)) {}
    sivm_status(&sivm);

    /*
    mot mem[] = {
        { .codage = { ADD,  REGREG, 3, 4 }},
        { .codage = { LOAD, REGDIR, 0, 2 }},
        { .brut   =   1000                },
        { .codage = { JMP,  REGIMM       }},
        { .brut   =   1500                }
    };
    //displayStatus();
    unsigned int i;
    unsigned int taille = sizeof(mem)/sizeof(mot);
    for (i = 0; i < taille; ++i)
    {
        mot *m = &mem[i];
        
        if (!checkModes(m))
            quit("mode incorrect");

        if (m->codage.codeop == ADD)
        {
            if (m->codage.mode == REGREG)
                REG[m->codage.dest] += REG[m->codage.source];
            else if (m->codage.mode == REGIMM)
                REG[m->codage.dest] += m->codage.source;
        }
    }

    displayStatus();
*/
    return 0;
}
