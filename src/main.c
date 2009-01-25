/**
 *
 *  Questions :
 *  1) Quelles options de compilations doit-on utiliser (std=c99, Wall, Werror, ansi) ?
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "mot.h"
#include "util.h"

/*const unsigned int NB_REG = 8;*/
#define NB_REG 8
uint16_t PC = 0;
uint16_t SP = 0;
uint16_t SR = 0;
uint16_t REG[NB_REG] = {0};

void displayStatus()
{
    printf("==> Status <==\n");
    unsigned int i;
    for (i = 0; i < NB_REG; ++i)
        printf("\tREG[%d] = %d\n", i, REG[i]);
}

int main(int argc, char *argv[])
{
    REG[3] = 1;
    REG[4] = 2;
    /*
    mot mem[] = {
        { .codage = { ADD,  REGREG, 3, 4 }},
        { .codage = { LOAD, REGDIR, 0, 2 }},
        { .brut   =   1000                },
        { .codage = { JMP,  REGIMM       }},
        { .brut   =   1500                }
    };
    */

    mot mem[] = {
        { .codage = { ADD, REGREG, 3, 4 }},
        { .codage = { ADD, REGREG, 4, 1 }}
    };

    displayStatus();

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

    return 0;
}
