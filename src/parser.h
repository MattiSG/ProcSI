#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "sivm.h"
#include "util.h"
#include "instructions.h"

/**@name	Label lists*/
//@{

/**Chained list of labels
 *Helps the assembleur to remember at which address labels are located.
 *A label is caractised by it's name, and it's address (or pointer)
 */
typedef struct _LblListElm LblListElm;
struct _LblListElm {
    char *name;         /*!< name of the label */
    REG pointer;        /*!< where the label is located */

    LblListElm *next;   /*!< pointer to next label struct */
};

/**Adds a label
 *@param    head    pointer to the head of the list (NULL is empty / no list)
 *@param    ptr     the label's address
 *@param    name    label's name
 *@param    len     len of the label name
 *@returns	Address to the new head of list
 */
LblListElm *lbllist_add(LblListElm *head, REG ptr, char *name, size_t len);

/**Get the next link in the chained list of labels
 *@param    current pointer to the element you want to get the next one
 *@returns	Address of the next element. NULL if end of list
 */
LblListElm *lbllist_next(LblListElm *current);

/**Pick a label element from the list
 *@param    head    pointer to the head of the list
 *@param    name    label's name
 *@param    len     len of the label name
 *@param    pointer output pointer to label's address
 *@returns	true if element is found and pointer is written. false is no corresponding element found
 */
bool lbllist_get(LblListElm *head, char *name, size_t len, REG *pointer);
//@}

typedef struct
{
    int memsize; /*!< code's size in memory */
    cmd_word *mem; /*!< cmd_word array of size memsize, containing assembled code */

    LblListElm *labels_head; /*!< head of chained list of labels */

    int* pcline;                /*!< array making corresps a pc as index to the line */
} ParserResult;

/**Parse a procsi assembly file, and loads it in a virtual machine
 *Since mem is allocated in this function, it's up to you to free it when not needed anymore
 *@see      svim_parse
 *@param    presult  pointer to parser result. modified if parsed
 *@param    file     the filename of the source code
 *@returns	false if any error occured (stderr is written in consequence with perror), true if the file is assembled and loaded corretly.
 */
bool sivm_parse_file(ParserResult *presult, char *file);

/**Save the program into a file in a binary format
 *@param    filename file output
 *@param    mem      program
 *@param    memsize  size of the program
 */
void save_program(char *filename, cmd_word mem[], int memsize);

/**Load a program into the memory from a binary file
 *@param    filename file input
 *@param    mem      pointer to the program
 *@param    memsize  size of the program
 */
void load_program(char *filename, cmd_word *mem[], int *memsize);

#endif /* PARSER_H */
