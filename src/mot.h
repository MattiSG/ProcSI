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

typedef enum
{
    REGREG = 0b0000,
    REGIMM = 0b0100,
    REGDIR = 0b1000,
    REGIND = 0b1100,
    DIRIMM = 0b0101,
    DIRREG = 0b0001,
    INDIMM = 0b0110,
    INDREG = 0b0010
} mode;

enum instruction
{
    LOAD = 0,
    STORE,
    ADD,
    SUB,
    JMP,
    JEQ,
    CALL,
    RET,
    PUSH,
    POP,
    HALT
};

typedef enum
{
    FM_REGREG = 1 << REGREG,
    FM_REGIMM = 1 << REGIMM,
    FM_REGDIR = 1 << REGDIR,
    FM_REGIND = 1 << REGIND,
    FM_DIRIMM = 1 << DIRIMM,
    FM_DIRREG = 1 << DIRREG,
    FM_INDIMM = 1 << INDIMM,
    FM_INDREG = 1 << INDREG
} f_mode;

typedef struct {
    REG pc;
    REG sp;
    REG sr;
    REG reg[NREGS];
    mot mem[MEMSIZE];
} SIVM;


typedef struct {
    bool (*function)(SIVM *sivm, mot *m);
    f_mode modes;
} Instr;

bool checkModes(mot *m);
/**
 * \brief Initializes a new ProcSI virtual machine
 * \author Me
 */
bool sivm_new(SIVM *sivm);
bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize]);
bool sivm_step(SIVM *sivm);
void sivm_status(SIVM *sivm);

#endif /*MOT_H*/
