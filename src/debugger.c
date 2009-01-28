#include <stdio.h>
#include <stdbool.h>

#include "debugger.h"
#include "instructions.h"
#include "util.h"

typedef enum
{
    RUN,
    STEP,
    QUIT,
    BREAKPOINT,
    DISPLAY,
    RESTART,
    HELP
} commands;

void debugger_new(Debugger *debug)
{
    sivm_new(&debug->sivm);
}

void debugger_start(Debugger *debug)
{
    /*
    printf("Quel fichier voulez-vous utiliser ? ");
    fflush(stdout);

    char filename[LINE_MAX];
    readLine(filename, LINE_MAX);
    printf("Fichier : \"%s\"\n", filename);
    */

    /* mot prg = analyzer(filename); */

    /*
    mot prg[] = {
        { .codage = { ADD,  REGREG, 3, 4 }},
        { .codage = { LOAD, REGDIR, 0, 2 }},
        { .brut   =   1000                },
        { .codage = { JMP,  REGIMM       }},
        { .brut   =   1500                }
    };
    */
    mot prg[] = {
        { .codage = { ADD, REGREG, 3, 4 }},
        { .codage = { ADD, REGREG, 4, 1 }},
		{ .codage = { HALT				}}
    };

    sivm_load(&debug->sivm, sizeof(prg) / sizeof(mot), prg);
    debug->sivm.reg[3] = 1;
    debug->sivm.reg[4] = 2;
    sivm_status(&debug->sivm);
    while (sivm_step(&debug->sivm))
		sivm_status(&debug->sivm);
}

bool step_by_step = false;
bool stop = false;


int parse_command(char *cmd)
{
    if (strcmp(cmd, "run"))
        return RUN;
    if (strcmp(cmd, "step"))
        return STEP;
    if (strcmp(cmd, "quit"))
        return QUIT;
    if (strcmp(cmd, "breakpoint"))
        return BREAKPOINT;
    if (strcmp(cmd, "display"))
        return DISPLAY;
    if (strcmp(cmd, "restart"))
        return RESTART;
    return HELP;
}

void interpret()
{
    char cmd[MAX_INPUT];
    switch(parse_command(cmd))
    {
        case RUN:
            step_by_step = false;
            break;
        case STEP:
            step_by_step = true;
            break;
        case QUIT:
            stop = true;
            break;
        case BREAKPOINT:
            /* nothing */
            break;
        case DISPLAY:
            /* nothing */
            break;
        case RESTART:
            //pc = 0;
            break;
    }
}

void boucle()
{
    while(!stop)
    {
        //stop = sivm_step();
        if (step_by_step)
            interpret();
    }
}
