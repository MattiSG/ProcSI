#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdbool.h>

typedef struct breakpoint_t
{
    unsigned int num;
    unsigned int line;
    struct breakpoint_t *next;
} breakpoint;

typedef struct
{
    unsigned int size;
    unsigned int count;
    breakpoint *head;
} breakpoints_list;

void breakpoint_list_new(breakpoints_list *list);
bool breakpoint_list_has(breakpoints_list *list, unsigned int line);
breakpoint* breakpoint_list_get(breakpoints_list *list, unsigned int index);
bool breakpoint_list_add(breakpoints_list *list, unsigned int line);
bool breakpoint_list_rm(breakpoints_list *list, unsigned int num);
void breakpoint_list_display(breakpoints_list *list);

#endif /*BREAKPOINT_H*/
