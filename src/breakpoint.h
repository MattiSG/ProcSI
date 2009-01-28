#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdbool.h>

typedef struct breakpoint
{
    unsigned int line;
    struct breakpoint *next;
} breakpoint_t;

typedef struct
{
    unsigned int size;
    struct breakpoint *head;
} breakpoints_list;

bool hasBreakpoint(breakpoints_list *list, unsigned int line);
bool addBreapoint(breakpoints_list *list, unsigned int line);
bool removeBreapoint(breakpoints_list *list, unsigned int line);

#endif /*BREAKPOINT_H*/
