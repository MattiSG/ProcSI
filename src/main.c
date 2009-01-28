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

#include "debugger.h"

int main(int argc, char *argv[])
{
    Debugger debug;
    debugger_new(&debug);
    debugger_start(&debug);

    return 0;
}
