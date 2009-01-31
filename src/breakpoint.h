#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdbool.h>

/**
 * @struct breakpoint
 * @brief  Structure to reference a breakpoint
 */
typedef struct breakpoint_t
{
    unsigned int num;           /*!< breakpoint's number */
    unsigned int line;          /*!< number of line */
    struct breakpoint_t *next;  /*!< chained list */
} breakpoint;

/**
 * @struct breakpoints_list
 * @brief  Structure to reference a debugger command
 */
typedef struct
{
    unsigned int size;          /*!< number of breakpoints */
    unsigned int count;         /*!< create a uniq id for breakpoint */
    breakpoint *head;           /*!< start of chained list */
} breakpoints_list;

/**
 * @brief Initialize a breakpoints_list
 * @param list  pointer to a breakpoint_list
 */
void breakpoint_list_new(breakpoints_list *list);

/**
 * @brief Look for a breakpoint with this line
 * @param list  pointer to a breakpoint_list
 * @param line  number of line
 */
bool breakpoint_list_has(breakpoints_list *list, unsigned int line);

/**
 * @brief Accessor for a breakpoint
 * @param list  pointer to a breakpoint_list
 * @param index number of the index in the chained list
 */
breakpoint* breakpoint_list_get(breakpoints_list *list, unsigned int index);

/**
 * @brief Add a new breakpoint to the chained list
 * @param list  pointer to a breakpoint_list
 * @param line  number of line
 */
bool breakpoint_list_add(breakpoints_list *list, unsigned int line);

/**
 * @brief Remove a breakpoint from the chained list
 * @param list  pointer to a breakpoint_list
 * @param num   number of breakpoint
 */
bool breakpoint_list_rm(breakpoints_list *list, unsigned int num);

/**
 * @brief Display the list of breakpoint with their number and line
 * @param list  pointer to a breakpoint_list
 */
void breakpoint_list_display(breakpoints_list *list);

#endif /*BREAKPOINT_H*/
