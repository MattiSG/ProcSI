#ifndef SIVM_H
#define SIVM_H

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

typedef uint16_t REG;

/**@name	SIVM parameters*/
//@{
/**Number of registers in an SIVM.*/
#define NREGS 8
/**Size of an SIVM's memory*/
#define MEMSIZE 128
/**PC index at SIVM startup*/
#define PC_START 0
/**SR index at SIVM startup*/
#define SR_START 0
/**SP index at SIVM startup*/
#define SP_START 0
//@}

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
void sivm_new(SIVM *sivm);
bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize]);
bool sivm_step(SIVM *sivm);
void sivm_status(SIVM *sivm);

#endif /*SIVM_H*/
