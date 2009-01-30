#include "parser.h"
#include "instructions.h"
#include <ctype.h>

/**Parser structure. Helps during parsing
 *It's role is to keep information needed during the parsing, for internal purposes
 */
typedef struct {
    /* per char parsing */
    char *cur;          /*!< pointer to currently being parsed character */
    unsigned int row;   /*!< currently being parsed row */
    unsigned int col;   /*!< currently being parser column */

    REG pc;             /*!< currently location in memory */
    cmd_word* mem;      /*!< the whole memory */
    int* pcline;        /*!< pcline[pc] => .procsi line (size of memsize) */
    REG memsize;        /*!< size of the memory */
    FILE* fp;           /*!< pointer to .procsi FILE */
    
    LblListElm *labels; /*!< labels, and their corresponding address */
} Parser;

/**Pseudo-modes list
 *Since the parser have to read one instrution's attribute at one, we need to have invidual parameter for theses.
 */
typedef enum {
    PM_REG,     // RX
    PM_IMM,     // #X
    PM_IND,     // [RX]
    PM_DIR,     // [X]
    //PM_DEP    // X[RY]   //TODO
} PMode;

LblListElm *lbllist_add(LblListElm *head, REG ptr, char *name, size_t len)
{
    LblListElm *newhead = malloc(sizeof(*newhead));

    newhead->next = head;
    newhead->pointer = ptr;
    newhead->name = malloc(len + 1);
    strncpy(newhead->name, name, len);

    return newhead;
}

LblListElm *lbllist_next(LblListElm *current)
{
    return current->next;
}

bool lbllist_get(LblListElm *head, char *name, size_t len, REG *pointer)
{
    for(LblListElm* lbl = head; lbl; lbl = lbllist_next(lbl))
    {
        if(!strncmp(lbl->name, name, len))
        {
            *pointer = lbl->pointer;
            return true;
        }
    }

    return false;
}

/**Transforms two pseudo-modes to a complete mode
 *This function is using switches since we cannot base on cutting the mode in two bits
 *@param    parser   pointer to a parser structure
 *@param    spmode   source pseudo-mode
 *@param    dpmode   destination pseudo-mode
 *@see	    parser.c#enum PMode
 *@see	    instructions.h#enum mode
 *@returns  A mode ready to be placed into a mot structure
 */
mode pseudomode_to_mode(Parser *parser, PMode spmode, PMode dpmode)
{
    switch(dpmode)
    {
    case PM_REG:
        switch(spmode)
        {
        case PM_REG:
            return REGREG;
        case PM_IMM:
            return REGIMM;
        case PM_DIR:
            return REGDIR;
        case PM_IND:
            return REGIND;
        }
        break;
    case PM_IND:
        switch(spmode)
        {
        case PM_REG:
            return INDREG;
        case PM_IMM:
            return INDIMM;
        default:
            logm(LOG_FATAL_ERROR, "Mode can't be INDDIR / INDIND at %d:%d",
                 parser->row, parser->col);
        }
        break;
    case PM_IMM:
        logm(LOG_FATAL_ERROR, "Mode can't be IMMREG / IMMIMM /"
             "IMMDIR / IMMIND at %d:%d",
             parser->row, parser->col);
    case PM_DIR:
        switch(spmode)
        {
        case PM_REG:
            return DIRREG;
        case PM_IMM:
            return DIRIMM;
        default:
            logm(LOG_FATAL_ERROR, "Mode can't be DIRDIR / DIRIND at %d:%d",
                 parser->row, parser->col);
        }
        break;
    }
    return REGISTER;
}

/**Converts an ascii character to it's value according to base
 *Only supported bases are base 2, 10 and 16
 *@param   ascii   ascii value of number to convert
 *@param   base    destination base
 *@returns The value of the character corresponding the base
 */
char ascii2base(char ascii, int base)
{
    switch(base)
    {
    case 16:
        if((ascii >= 'a' && ascii <= 'f') || (ascii >= 'A' && ascii <= 'F'))
            return tolower(ascii) - 'a';
    case 2:
    case 10:
        return ascii - '0';
        break;
    default:
        return 0;
    }
}

/**Parses a number
 *@param    parser      a pointer to the Parser structure
 *@param    value       a pointer to an output int
 *@param    modifiable  tells if the numbers base can be modifier (0xY, Yh)
 *@returns	false if no number can be read
 */
bool parse_number(Parser* parser, int* value, bool modifiable)
{
    int base = 10;

    // should here be expecting a number (even a register number)
    if(!isdigit(*parser->cur))
    {
        if(modifiable)
        {
            // binary
            if(*parser->cur == 'b' &&
               (*(parser->cur+1) == '0' || *(parser->cur+1) == '1'))
            {
                base = 2;
                parser->cur++;
                parser->col++;
            }
            // label
            else
            {
                REG reg;
                if(lbllist_get(parser->labels, parser->cur,
                               strlen(parser->cur) - 1, &reg))
                {
                    *value = reg;
                    return true;
                }
            }
        }
        else
        {
            logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
                 parser->row, parser->col, *parser->cur);
            return false;
        }
    }
    
    // hexa
    if(*parser->cur == '0' && *(parser->cur + 1) == 'x')
    {
        base = 16;
        parser->cur += 2;
        parser->col += 2;
    }
    
    // initialize
    *value = 0;
    
    // read the number
    for(; *parser->cur && isdigit(*parser->cur); parser->col++,parser->cur++)
    {
        *value = *value * base + ascii2base(*parser->cur, base);
    }

    return value;
}

/**Parses an instruction attribute
 *@param    parser      pointer to the Parser structure
 *@param    pmode       pointer to the mode
 *@param    reg         pointer to the register number
 *@param    data        pointer to the data (for direct/immediate modes)
 *@returns	false if no attrib can be read
 */
bool parse_attrib(Parser *parser, PMode *pmode, int *data, int *reg)
{
    bool ispointer = false,
         isregister = false,
         isimmediate = false;
    int n;
    
    // skipy
    for(; isblank(*parser->cur); parser->cur++,parser->col++)
    {}
    
    // indirect and direct mode are specified by [ ]
    if(*parser->cur == '[')
    {
        ispointer = true;
        parser->col++;
        parser->cur++;
    }
    
    // immediat mode is specified by precessing the number by #
    if(*parser->cur == '#' && ispointer == false)
    {
        isimmediate = true;
        parser->col++;
        parser->cur++;
    }
    
    if(isalpha(*parser->cur))
    {
        // it is a bird, is it a plane ? no, it's a register
        if((*parser->cur == 'R' || *parser->cur == 'r') &&
           isdigit(*(parser->cur+1)))
        {
            isregister = true;
            parser->col++;
            parser->cur++;
        }
    }
    
    // read the number
    if(!parse_number(parser, &n, !isregister))
    {
        logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
             parser->row, parser->col, *parser->cur);
        return false;
    }

    // now guess the mode
    if(isregister)
    {
        *reg = n;
        if(ispointer)
            *pmode = PM_IND;
        else
            *pmode = PM_REG;
    }
    else
    {
        *data = n;
        if(ispointer)
            *pmode = PM_DIR;
        else
            *pmode = PM_IMM;
    }
    
    // indrect and direct mode are closed by ']'
    if(ispointer)
    {
        for(; isblank(*parser->cur); parser->cur++,parser->col++)
        {}

        if(*parser->cur != ']')
        {
            logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
                 parser->row, parser->col, *parser->cur);
            return false;
        }
    }

    return true;
}

/**Parses instructions having just a destination
 *@param    parser      pointer to the Parser structure
 *@param    m           output array containing the instruction (and value if needed)
 *@param    instrsize   size of the instruction, and then size of m array
 *@returns	false if any error occurs
 */
bool parse_dest(Parser* parser, cmd_word m[3], unsigned int *instrsize)
{
    // dest
    int ddata, dreg;
    PMode dpmode;

    // assert there is at least one whitespace before
    if(!isblank(*parser->cur))
    {
        logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
             parser->row, parser->col, *parser->cur);
        return false;
    }

    // read the destination
    if(!parse_attrib(parser, &dpmode, &ddata, &dreg))
    {
        return false;
    }
    if(dpmode == PM_REG || dpmode == PM_IND)
        m[0].codage.dest = dreg;
    if(dpmode == PM_IMM || dpmode == PM_DIR)
        m[(*instrsize)++].brut = ddata;
    
    // guess the mode
    switch(dpmode)
    {
    case PM_REG:
    case PM_IND:
    case PM_DIR:
        m[0].codage.mode = pseudomode_to_mode(parser, PM_REG, dpmode);
        break;
    case PM_IMM:
        m[0].codage.mode = pseudomode_to_mode(parser, PM_DIR, dpmode);
        break;
    }

    for(; isblank(*parser->cur); parser->col++,parser->cur++)
    {}
    
    return true;
}

/**Parses instructions having just a source
 *@param    parser      pointer to the Parser structure
 *@param    m           output array containing the instruction (and value if needed)
 *@param    instrsize   size of the instruction, and then size of m array
 *@returns	false if any error occurs
 */
bool parse_source(Parser* parser, cmd_word m[3], unsigned int *instrsize)
{
    // source
    int sdata, sreg;
    PMode spmode;

    // assert there is at least one whitespace before
    if(!isblank(*parser->cur))
    {
        logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
             parser->row, parser->col, *parser->cur);
        return false;
    }

    // read the source
    if(!parse_attrib(parser, &spmode, &sdata, &sreg))
    {
        return false;
    }
    if(spmode == PM_REG || spmode == PM_IND)
        m[0].codage.source = sreg;
    if(spmode == PM_IMM || spmode == PM_DIR)
        m[(*instrsize)++].brut = sdata;
    
    // guess the mode
    m[0].codage.mode = pseudomode_to_mode(parser, spmode, PM_REG);

    for(; isblank(*parser->cur); parser->col++,parser->cur++)
    {}
    
    return true;
}

/**Parses instructions having destination and source (in that order)
 *@param    parser      pointer to the Parser structure
 *@param    m           output array containing the instruction (and value(s) if needed)
 *@param    instrsize   size of the instruction, and then size of m array
 *@returns	false if any error occurs
 */
bool parse_destsource(Parser* parser, cmd_word m[3], unsigned int *instrsize)
{
    // source
    int sdata, sreg;
    PMode spmode;

    // destination
    int ddata, dreg;
    PMode dpmode;

    // assert there is at least one whitespace before
    if(!isblank(*parser->cur))
    {
        logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
             parser->row, parser->col, *parser->cur);
        return false;
    }

    // read the destination
    if(!parse_attrib(parser, &dpmode, &ddata, &dreg))
    {
        return false;
    }
    if(dpmode == PM_REG || dpmode == PM_IND)
        m[0].codage.dest = dreg;
    if(dpmode == PM_IMM || dpmode == PM_DIR)
        m[(*instrsize)++].brut = ddata;
    
    // assert there is a coma beteen source and destination
    if (*parser->cur == ',')
    {
        parser->cur++;
        parser->col++;
    }
    else
    {
        logm(LOG_ERROR, "Unexpected token at %d:%d : `%c'",
             parser->row, parser->col, *parser->cur);
        return false;
    }

    for(; isblank(*parser->cur); parser->col++,parser->cur++)
    {}

    // read the source
    if(!parse_attrib(parser, &spmode, &sdata, &sreg))
    {
        return false;
    }
    if(spmode == PM_REG || spmode == PM_IND)
        m[0].codage.source = sreg;
    if(spmode == PM_IMM || spmode == PM_DIR)
        m[(*instrsize)++].brut = sdata;

    for(; isblank(*parser->cur); parser->col++,parser->cur++)
    {}

    // guess the mode
    m[0].codage.mode = pseudomode_to_mode(parser, spmode, dpmode);

    return true;
}

/**Parses a single instruction knowing it's opcode
 *@param    parser      pointer to the Parser structure
 *@param    m           output array containing the instruction (and value if needed)
 *@param    instrsize   size of the instruction, and then size of m array
 *@returns	false if any error occurs
 */
bool parse_instruction(Parser* parser, cmd_word m[3], unsigned int *instrsize)
{
    Instr instr = getInstruction(m[0]);
    if (instr.source)
	{
		if (instr.destination)
			return parse_destsource(parser, m, instrsize);
		else
			return parse_source(parser, m, instrsize);
    }
    else
    {
        if (instr.destination)
            return parse_dest(parser, m, instrsize);
    }
	return true;
}

/**Parses one line of assembly code
 *@param    parser      pointer to the Parser structure
 *@param    line        pointer to line's string
 *@returns	false if any error occurs
 */
bool parse_pass_line(Parser* parser, char *line)
{
    char instr[256];
    cmd_word m[3];
    unsigned int instrsize;

    parser->cur = line;
    parser->col = 1;

    // skipy
    for(; isblank(*parser->cur);
        parser->col++, parser->cur++)
    {}

    // read the instructions name
    if(sscanf(parser->cur, "%s", instr) == 1)
    {
        int len = strlen(instr);
    
        // in case there is a label just before some code on a the same line
        if(instr[len - 1] == ':')
        {
            parser->cur += len;
            parser->col += len;

            return parse_pass_line(parser, parser->cur);
        }

        instrsize = 1;
        parser->cur += len;
        parser->col += len;

        if(!strcmp(instr, "load"))
            m[0].codage.codeop = LOAD;

        else if(!strcmp(instr, "store"))
            m[0].codage.codeop = STORE;

        else if(!strcmp(instr, "add"))
            m[0].codage.codeop = ADD;

        else if(!strcmp(instr, "sub"))
            m[0].codage.codeop = SUB;

        else if(!strcmp(instr, "and"))
            m[0].codage.codeop = AND;

        else if(!strcmp(instr, "or"))
            m[0].codage.codeop = OR;

        else if(!strcmp(instr, "shl"))
            m[0].codage.codeop = SHL;

        else if(!strcmp(instr, "shr"))
            m[0].codage.codeop = SHR;

        else if(!strcmp(instr, "jmp"))
            m[0].codage.codeop = JMP;

        else if(!strcmp(instr, "jeq"))
            m[0].codage.codeop = JEQ;

        else if(!strcmp(instr, "call"))
            m[0].codage.codeop = CALL;

        else if(!strcmp(instr, "ret"))
            m[0].codage.codeop = RET;

        else if(!strcmp(instr, "push"))
            m[0].codage.codeop = PUSH;

        else if(!strcmp(instr, "pop"))
            m[0].codage.codeop = POP;

        else if(!strcmp(instr, "mov"))
            m[0].codage.codeop = MOV;
        
        else if(!strcmp(instr, "halt"))
            m[0].codage.codeop = HALT;

        else
        {
            if (instr[0] == ';' || instr[0] == '#')
            {
                return true;
            }

            logm(LOG_ERROR, "Unknown instruction `%s'", instr);
        }
        
        if(!parse_instruction(parser, m, &instrsize))
            return false;
        
        // write instruction into the memory
        if(instrsize > 0)
        {
            if(parser->mem)
            {
                if ((getInstruction(m[0]).source || getInstruction(m[0]).destination) && !checkModes(m[0]))
                {
                    logm(LOG_ERROR, "Invalid mode for instruction "
                         "at line %d",
                         parser->row);
                    return false;
                }
                
                // second pass
                for(int i = 0; i < instrsize; i++)
                {
                    parser->pcline[parser->pc] = parser->row;
                    parser->mem[parser->pc++].brut = m[i].brut;
                }
            }
            else
            {
                // first pass
                parser->pc += instrsize;
            }
        }
    }

    return true;
}

/**Parses a first time the whole source code for branches
 *@param    parser      pointer to the Parser structure
 *@returns	false if any error occurs
 */
bool parse_first_pass(Parser* parser)
{
    char line[LINE_MAX];

    parser->labels = NULL;
    parser->mem = NULL;
    parser->row = 0;
    parser->pc = 0;
    
    // read the buffer line by line
    while(fgets(line, sizeof(line), parser->fp) != NULL)
    {
        char instr[256];
        
        parser->row++;
        parser->cur = line;
        parser->col = 1;
        
        // read the instructions name
        if(sscanf(parser->cur, "%s", instr) == 1)
        {
            int len = strlen(instr);
            if (instr[len - 1] == ':')
            {
               parser->labels = lbllist_add(parser->labels, parser->pc, instr,
                                            len - 1);
            }
        }

        // we need to parse instructions, because we need to know the exact
        // addresse we are
        if(!parse_pass_line(parser, line))
        {
            return false;
        }
    }

    parser->memsize = parser->pc;
    // allocate the memory correspind size needed to write the code
    parser->mem = malloc(parser->memsize * sizeof(parser->mem[0]));
    parser->pcline = malloc(parser->memsize * sizeof(parser->pcline[0]));
    
    return true;
}

/**Parses a second time the source, to assemble
 *@param    parser      pointer to the Parser structure
 *@returns	false if any error occurs
 */
bool parse_second_pass(Parser* parser)
{
    char line[LINE_MAX];
    
    parser->row = 0;
    parser->pc = 0;
    
    // read the buffer line by line
    while(fgets(line, sizeof(line), parser->fp) != NULL)
    {
        parser->row++;
        parser->cur = line;
        parser->col = 1;
        
        if (!parse_pass_line(parser, line))
        {
            return false;
        }
    }

    return true;
}

bool sivm_parse(ParserResult *presult, FILE* f)
{
    Parser parser;
    
    parser.fp = f;
    
    if(!parse_first_pass(&parser))
    {
        return false;
    }
    
    rewind(f);

    if(!parse_second_pass(&parser))
    {
        return false;
    }

    // write
    presult->memsize = parser.memsize;
    presult->mem = parser.mem;
    presult->pcline = parser.pcline;
    presult->labels_head = parser.labels;

    return true;
}

bool sivm_parse_file(ParserResult *presult, char *file)
{
    bool ret;
    FILE* f;

    f = fopen(file, "r");
    if(f == NULL) {
        logm(LOG_ERROR, "Can't open file `%s'", file);
        perror("fopen");
        return false;
    }

    // let's parse!
    ret = sivm_parse(presult, f);

    fclose(f);

    return ret;
}

void save_program(char *filename, cmd_word mem[], int memsize)
{
    FILE *f = fopen(filename, "wb");
    fprintf(f, "%d\n", memsize);
    fwrite(mem, memsize, sizeof(cmd_word), f);
    fclose(f);
}

void load_program(char *filename, cmd_word *mem[], int *memsize)
{
    FILE *f = fopen(filename, "rb");
    char buf[LINE_MAX];
    fgets(buf, LINE_MAX, f);
    *memsize = atoi(buf);
	*mem     = malloc(sizeof(cmd_word) * (*memsize));
    fread(*mem, sizeof(cmd_word), (*memsize), f);
    fclose(f);
}
