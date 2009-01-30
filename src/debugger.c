#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "debugger.h"
#include "breakpoint.h"
#include "instructions.h"
#include "util.h"
#include "cmd_word.h"

typedef struct
{
    char *name;
    char *help;
} Command;

typedef enum
{
    RUN = 0,
    STEP,
    QUIT,
    STATUS,
    RESTART,
    HELP,
    DISPLAY,
    BREAKPOINT,
    UNKNOWN
} type_command;

//const unsigned int NB_COMMANDS = 8;
#define NB_COMMANDS 8
Command commands[NB_COMMANDS] = {
    [RUN]        = { "run", "run program without step by step" },
    [STEP]       = { "step", "make a step in the program" },
    [QUIT]       = { "quit", "close the debugger" },
    [STATUS]     = { "status", "display registers" },
    [RESTART]    = { "restart", "restart the program" },
    [HELP]       = { "help", "display help" },
    [DISPLAY]    = { "display", "display a variable or a mem" },
    [BREAKPOINT] = { "breakpoint", "put or delete a breakpoint" }
};

int parseCommand(char *cmd)
{
    for (unsigned int i = 0; i < NB_COMMANDS; ++i)
        if (!strncmp(cmd, commands[i].name, strlen(commands[i].name)) && strlen(cmd) == strlen(commands[i].name))
            return i;

    return UNKNOWN;
}

void displayHelp()
{
    for (unsigned int i = 0; i < NB_COMMANDS; ++i)
        printf("  %s : %s\n", commands[i].name, commands[i].help);
}

void debugger_new(Debugger *debug)
{
    char filename[LINE_MAX];

    sivm_new(&debug->sivm);

    /*
    printf("Quel fichier voulez-vous utiliser ? ");
    fflush(stdout);

    readLine(filename, LINE_MAX);
    printf("Fichier : \"%s\"\n", filename);
    */
    strcpy(filename, "examples/test.procsi");


	cmd_word *prg;
    int memsize;
    if(!sivm_parse_file(&memsize, &prg, filename))
    {
        quit("Can't load / assemble file");
    }
    //printf("%d %d %d %d\n", prg[0].codage.codeop, prg[0].codage.mode, prg[0].codage.source, prg[0].codage.dest);
    //printf("taille => %d\n", memsize);
 
 

/*	
    cmd_word prg[] = {
        { .codage = { MOV, REGIMM, 3, 0 }},
        { .brut   =   1					  },
        { .codage = { MOV, REGIMM, 0, 4 }},
        { .brut   =   2					  },
        { .codage = { ADD,  REGREG, 3, 4 }},
        { .codage = { LOAD, REGDIR, 4, 2 }},
        { .brut   =   MEMSIZE - 1         },
        { .codage = { JMP,  REGIMM       }},
        { .brut   =   1500                },
		{ .codage = { HALT				 }}
    };
    int memsize = sizeof(prg) / sizeof(cmd_word);
*/	
    printf("Loaded program:\n%s", disassemble(memsize, prg));
    sivm_load(&debug->sivm, memsize, prg);
}

void debugger_start(Debugger *debug)
{
    breakpoints_list breakpoints;
    breakpoint_list_new(&breakpoints);

    bool end_found = false;
    bool step_by_step = true;
    bool execute = false;
    bool finish = false;
    do
    {
        printf("$ ");
        fflush(stdout);

        char line[LINE_MAX];
        readLine(line, LINE_MAX);
        char *cmd = strtok(line, " ");
        //printf("token => %s\n", line);

        switch (parseCommand(cmd))
        {
            case RUN:
                step_by_step = false;
                execute = true;
                break;
            case STEP:
                step_by_step = true;
                execute = true;
                break;
            case STATUS:
                execute = false;
                sivm_status(&debug->sivm);
                break;
            case QUIT:
                finish = true;
                execute = false;
                break;
            case RESTART:
                debugger_new(debug);
                end_found = false;
                step_by_step = true;
                execute = false;
                break;
            case DISPLAY:
                {
                    execute = false;
                    char *type = strtok(0, " ");
                    if (!type || strlen(type) != 3)
                    {
                        printf("Usage: display (reg|mem) number\n");
                        break;
                    }
                    char *num = strtok(0, " ");
                    if (!num || !isdigit(num[0]))
                    {
                        printf("Usage: display (reg|mem) number\n");
                        break;
                    }
                    unsigned int nb = atoi(num);

                    if (!strcmp(type, "reg"))
                        printf("=> register %d: %d\n", nb, nb);
                    else if (!strcmp(type, "mem"))
                        printf("=> register %d: %d\n", nb, nb);
                    else
                        printf("Usage: display (reg|mem) number\n");
                }
                break;
            case BREAKPOINT:
                {
                    execute = false;
                    char *type = strtok(0, " ");
                    if (!type)
                    {
                        printf("breakpoints:\n");
                        breakpoint_list_display(&breakpoints);
                    }
                    else if (!strcmp(type, "add"))
                    {
                        char *num = strtok(0, " ");
                        if (!num || !isdigit(num[0]))
                        {
                            printf("Usage: breakpoint\nUsage: breakpoint add line\nUsage: breakpoint rm num\n");
                            break;
                        }
                        unsigned int nb = atoi(num);
                        if (!breakpoint_list_add(&breakpoints, nb))
                            printf("breakpoint already exists\n");
                        else
                            printf("added breakpoint at line %d\n", nb);
                    }
                    else if (!strcmp(type, "rm"))
                    {
                        char *num = strtok(0, " ");
                        if (!num || !isdigit(num[0]))
                        {
                            printf("Usage: breakpoint\nUsage: breakpoint add line\nUsage: breakpoint rm num\n");
                            break;
                        }
                        unsigned int nb = atoi(num);
                        if (!breakpoint_list_rm(&breakpoints, nb))
                            printf("no breakpoint\n");
                    }
                    else
                        printf("Usage: breakpoint\nUsage: breakpoint add line\nUsage: breakpoint rm num\n");
                }
                break;
            case UNKNOWN:
                printf("Unknown command\n");
            case HELP:
                execute = false;
                displayHelp();
                break;
        }

        while (execute)
        {
            if (end_found)
            {
                printf("End of program reached\n");
                break;
            }

            end_found = !sivm_step(&debug->sivm);

            if (step_by_step) break;
        }
    }
    while (!finish);
}
