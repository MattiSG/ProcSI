#include <stdbool.h>

#ifndef MOT_H
#define MOT_H

typedef union
{
    short brut;
    struct
    {
        unsigned codeop : 6;
        unsigned mode   : 4;
        unsigned source : 3;
        unsigned dest   : 3;
    } codage;
} mot;

enum mode
{
    REGREG = 0b0000,
    REGIMM = 0b0100,
    REGDIR = 0b1000,
    REGIND = 0b1100,
    DIRIMM = 0b0101,
    DIRREG = 0b0001,
    INDIMM = 0b0110,
    INDREG = 0b0010
};

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

bool checkModes(mot *m);

#endif /*MOT_H*/
