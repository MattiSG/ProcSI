#include <stdlib.h>

#include "breakpoint.h"

bool hasBreakpoint(breakpoints_list *list, unsigned int line)
{
    if (line > list->size) return false;

    for (breakpoint_t *b = list->head; b != 0; b = b->next)
        if (b->line == line)
            return true;

    return false;
}

bool addBreapoint(breakpoints_list *list, unsigned int line)
{
    if (hasBreakpoint(list, line)) return true;

    breakpoint_t *b = (breakpoint_t*) malloc(sizeof(breakpoint_t));
    b->line = line;
    b->next = list->head;

    list->head = b;
    ++list->size;

    return true;
}

bool removeBreapoint(breakpoints_list *list, unsigned int line)
{
    for (breakpoint_t *b = list->head, *bprev = 0; b != 0; bprev = b, b = b->next)
        if (b->line == line)
        {
            bprev->next = b->next;
            free(b);
            ++list->size;
            return true;
        }

    return false;
}
