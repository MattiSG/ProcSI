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
#include "util.h"
#include "cmd_word.h"

int main(int argc, char *argv[])
{
    // execute binary file
    if (argc == 2 && argv[1][0] != '-')
    {
        Debugger debug;
        debugger_new(&debug, argv[1], false);
        debugger_start(&debug);
    }
    // compile the source file in binary file
    else if (argc == 4 && (!strncmp("--compile", argv[1], 9) || !strncmp("-c", argv[1], 2)))
    {
        ParserResult presult;
        if (!sivm_parse_file(&presult, argv[3]))
            logm(LOG_FATAL_ERROR, "Unable to load / assemble file");
        save_program(argv[2], presult.mem, presult.memsize);
    }
    // execute source file
    else if (argc == 3 && (!strncmp("--source", argv[1], 8) || !strncmp("-s", argv[1], 2)))
    {
        Debugger debug;
        debugger_new(&debug, argv[2], true);
        debugger_start(&debug);
    }
    else
    {
        fprintf(stderr, "PROCSI emulator. Assemble, disassemble and execute PROCSI instructions.\n"
						"Authors: Romain Giraud, Clément Léger, Matti Schneider-Ghibaudo. W00T!!\n"
						"Usage: %s --compile, -c OUTPUT_FILE SOURCE_FILE\n"
                        "       %s --source, -s SOURCE_FILE\n"
                        "       %s BINARY_FILE\n", argv[0], argv[0], argv[0]);
        return 1;
    }

    return 0;
}
