/**
 *  
 *  SR reçoit le calcul après chaque instruction de calcul : ADD, SUB
 *
 *  Si codage en 3 mots, le 2ème mot est la src et le 3ème la dest.
 *
 *  Faire le debugger pas à pas + affichage des variables.
 *  Afficher aussi l'instruction lisible : ADD R1,R2 (par exemple)
 *
 *  Faire les erreurs d'éxecution
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "sivm.h"
#include "instructions.h"
#include "util.h"



int main(int argc, char *argv[])
{
    SIVM sivm;
    
    mot prgtest[] = {
        { .codage = { ADD, REGREG, 3, 4 }},
        { .codage = { ADD, REGREG, 4, 1 }}
    };
    /*
    mot prgtest[] = {
        { .codage = { ADD,  REGREG, 3, 4 }},
        { .codage = { LOAD, REGDIR, 0, 2 }},
        { .brut   =   1000                },
        { .codage = { JMP,  REGIMM       }},
        { .brut   =   1500                }
    };
    */

    sivm_new(&sivm);
    sivm_load(&sivm, sizeof(prgtest) / sizeof(prgtest[0]), prgtest);
    sivm.reg[3] = 1;
    sivm.reg[4] = 2;
    sivm_status(&sivm);
    while (sivm_step(&sivm)) {}
    sivm_status(&sivm);

    return 0;
}
