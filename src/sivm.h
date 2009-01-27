#ifndef MOT_H
#define MOT_H

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

typedef uint16_t REG;

#define NREGS 8
#define MEMSIZE (UINT16_MAX + 1)

typedef union
{
    REG brut;
    struct
    {
        unsigned codeop : 6;
        unsigned mode   : 4;
        unsigned source : 3;
        unsigned dest   : 3;
    } codage;
} mot;


typedef struct {
    REG pc;
    REG sp;
    REG sr;
    REG reg[NREGS];
    mot mem[MEMSIZE];
} SIVM;

/**
 * \brief Initializes a new ProcSI virtual machine
 * \author Me
 */
bool sivm_new(SIVM *sivm);
bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize]);
bool sivm_step(SIVM *sivm);
void sivm_status(SIVM *sivm);
bool sivm_exec(SIVM *sivm, mot *word);

#endif /*MOT_H*/
