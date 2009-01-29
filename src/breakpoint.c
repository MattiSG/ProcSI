#include <stdio.h>
#include <stdlib.h>

#include "breakpoint.h"

void breakpoint_list_new(breakpoints_list *list)
{
    list->size  = 0;
    list->count = 0;
    list->head  = 0;
}

void breakpoint_list_display(breakpoints_list *list)
{
    for (breakpoint *b = list->head; b != 0; b = b->next)
        printf("  #%d: line %d\n", b->num, b->line);
}

bool breakpoint_list_has(breakpoints_list *list, unsigned int line)
{
    //if (line > list->size) return false;

    for (breakpoint *b = list->head; b != 0; b = b->next)
        if (b->line == line)
            return true;

    return false;
}

breakpoint* breakpoint_list_get(breakpoints_list *list, unsigned int index)
{
    //if (index > list->size) return 0;

    for (breakpoint *b = list->head; b != 0; b = b->next)
        if (!index--)
            return b;

    return 0;
}

bool breakpoint_list_add(breakpoints_list *list, unsigned int line)
{
    if (breakpoint_list_has(list, line)) return false;

    breakpoint *b = (breakpoint*) malloc(sizeof(breakpoint));
    b->num  = ++list->count;
    b->line = line;
    b->next = 0;

    if (list->head)
    {
        breakpoint *prev;
        for (prev = list->head; prev->next != 0; prev = prev->next);
        prev->next = b;
    }
    else
    {
        list->head = b;
    }

    ++list->size;

    return true;
}
bool breakpoint_list_rm(breakpoints_list *list, unsigned int num)
{
    for (breakpoint *b = list->head, *bprev = 0; b != 0; bprev = b, b = b->next)
        if (b->num == num)
        {
            if (bprev)
                bprev->next = b->next;
            else
                list->head = b->next;
            free(b);
            --list->size;
            return true;
        }

    return false;
}
