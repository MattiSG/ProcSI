#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "debugger.h"
#include "breakpoint.h"
#include "instructions.h"
#include "util.h"
#include "cmd_word.h"

/**
 * \struct Command
 * \brief  Structure to reference a debugger command
 */
typedef struct
{
    char *name;
    char *help;
} Command;


/**
 * \enum   type_command
 * \brief  Enumerate all commands
 */
typedef enum
{
    RUN = 0,
    STEP,
	PROGRAM,
	INFO,
    RESTART,
    DISPLAY,
    BREAKPOINT,
    HELP,
    QUIT,
    UNKNOWN
} type_command;

#define NB_COMMANDS 9 /*!< number of commands */

/**
 * \brief Array of available commands
 * \see   Command
 */
Command commands[NB_COMMANDS] = {
    [RUN]        = { "run", "run the program all at once" },
    [STEP]       = { "step", "execute one instruction in the program" },
    [PROGRAM]    = { "program", "display the disassembled program currently loaded in the VM" },
    [INFO]       = { "info", "display specifications of the current VM" },
    [RESTART]    = { "reload", "reload the program (updates from the file)" },
    [DISPLAY]    = { "display", "display a register or memory unit value, or the whole VM status\n\tUsage: display [(reg number|PC|SP|SR) | (mem number)]" },
    [HELP]       = { "help", "display help" },
    [BREAKPOINT] = { "breakpoint", "add or remove a breakpoint" },
    [HELP]       = { "help", "display help" },
    [QUIT]       = { "quit", "close the debugger" }
};

/**
 * \fn    int find_command(char *cmd)
 * \brief find the command in the array of Command
 * \see   Command
 * \see   comands
 */
int find_command(char *cmd)
{
    for (unsigned int i = 0; i < NB_COMMANDS; ++i)
        if (! strncmp(cmd, commands[i].name, strlen(commands[i].name)) && strlen(cmd) == strlen(commands[i].name))
            return i;

    return UNKNOWN;
}

/**
 * \fn    void display_help()
 * \brief display all available commands and their help
 * \see   commands
 */
void display_help()
{
    for (unsigned int i = 0; i < NB_COMMANDS; ++i)
        printf((ANSI_OUTPUT ? "  \e[33m%s\e[0m:\t%s\n" : "  %s:\t%s\n"), commands[i].name, commands[i].help);
}

void debugger_new(Debugger *debug)
{
    char filename[LINE_MAX];
    /*
    printf("Quel fichier voulez-vous utiliser ? ");
    fflush(stdout);

    readLine(filename, LINE_MAX);
    printf("Fichier : \"%s\"\n", filename);
    */
    strcpy(filename, "examples/test.procsi");

	cmd_word *prg;
    int memsize;
    if (! sivm_parse_file(&memsize, &prg, filename))
        logm(LOG_FATAL_ERROR, "Unable to load / assemble file");
    else
        logm(LOG_STEP, "Parsing successful");
	
    sivm_new(&debug->sivm);
	debug->program = prg;
	debug->programSize = memsize;

/*	//A program loaded in memory has this form:
 
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
	if (ANSI_OUTPUT) printf("\e[32m");
    printf("Program loaded\n");
	if (ANSI_OUTPUT) printf("\e[0m");
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
        //printf("$ ");
        //fflush(stdout);
        //char line[LINE_MAX];
        //readLine(line, LINE_MAX);

        char *line = readline ("> ");
        char *cmd = strtok(line, " ");

        switch (find_command(cmd))
        {
            case RUN:
                step_by_step = false;
                execute = true;
                break;
            case STEP:
                step_by_step = true;
                execute = true;
				logm(LOG_INFO, sivm_get_instruction_string(&debug->sivm));
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
			case PROGRAM:
				printf(disassemble(debug->programSize, debug->program));
				printf("(Total size: %d words)\n", (int) debug->programSize);
				break;
			case INFO:
				if (ANSI_OUTPUT) {
					printf("\e[36mMemory size:\e[0m\n\t%d\n", MEMSIZE);
					printf("\e[36mNumber of registers:\e[0m\n\t%d\n", NREGS);
					printf("\e[36mReserved registers (not updated on CALL and RET):\e[0m\n\tR%d-R%d\n", PARAM_REGS_START, PARAM_REGS_END);
					printf("\e[36mStack is going through\e[0m %s adresses\n", (SP_INCR > 0 ? "ascending" : "descending"));
				} else {
					printf("Memory size:\n\t%d\n", MEMSIZE);
					printf("Number of registers:\n\t%d\n", NREGS);
					printf("Reserved registers (not updated on CALL and RET):\n\tR%d-R%d\n", PARAM_REGS_START, PARAM_REGS_END);
					printf("Stack is going through %s adresses\n", (SP_INCR > 0 ? "ascending" : "descending"));
				}
            case DISPLAY:
                {
                    execute = false;
                    char *type = strtok(0, " ");
                    if (!type || strlen(type) != 3)
                    {
                        sivm_status(&debug->sivm);
                        break;
                    }
                    char *num = strtok(0, " ");
                    if (!num)
                    {
						printf("Usage: %s\n", commands[DISPLAY].help);
                        break;
                    }
					
					if (! strcmp(type, "mem"))
					{
						if (! sivm_print_memory(&debug->sivm, atoi(num)))
							logm(LOG_WARNING, "Unreachable value. Size of memory for this VM is %d.", MEMSIZE);
					}
					else if (! strcmp(type, "reg"))
					{
						int reg;
						if ((! strcmp(num, "PC")) || (! strcmp(num, "pc")))
							reg = PC;
						else if ((! strcmp(num, "SR")) || (! strcmp(num, "sr")))
							reg = SR;
						else if ((! strcmp(num, "SP")) || (! strcmp(num, "sp")))
							reg = SP;
						else reg = atoi(num);
						
						if (! sivm_print_register(&debug->sivm, reg))
							logm(LOG_WARNING, "Unreachable value. Registers are available from 1 to %d, plus \"PC\", \"SP\" and \"SR\".", NREGS);
					}
                    else
						printf("Usage: %s\n", commands[DISPLAY].help);
                }
                break;
            case BREAKPOINT:
                {
                    execute = false;
                    char *type = strtok(0, " ");
                    if (!type)
                    {
                        if (breakpoints.size)
                        {
                            printf("breakpoints:\n");
                            breakpoint_list_display(&breakpoints);
                        }
                        else
                            printf("no breakpoint");
                    }
                    else if (!strcmp(type, "add"))
                    {
                        char *num = strtok(0, " ");
                        if (!num || !isdigit(num[0]))
                        {
                            printf("Usage: %s\n", commands[BREAKPOINT].help);
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
                            printf("Usage: %s\n", commands[BREAKPOINT].help);
                            break;
                        }
                        unsigned int nb = atoi(num);
                        if (!breakpoint_list_rm(&breakpoints, nb))
                            printf("no breakpoint\n");
                    }
                    else
                        printf("Usage: %s\n", commands[BREAKPOINT].help);
                }
                break;
            case UNKNOWN:
                printf("Unknown command\n");
            case HELP:
                execute = false;
                display_help();
                break;
        }
        add_history(line);

        while (execute)
        {
            if (end_found)
            {
                logm(LOG_STEP, "End of program reached\n");
                break;
            }

            end_found = !sivm_step(&debug->sivm);

            if (step_by_step) break;
        }
    }
    while (!finish);
}
