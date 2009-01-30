#include "parser.h"
#include <ctype.h>

typedef struct _LblListElm LblListElm;
struct _LblListElm {
    char *str;
    REG pointer;

    LblListElm *next;
};

LblListElm *lbllist_add(LblListElm *head, REG ptr, char *str, size_t len);
LblListElm *lbllist_next(LblListElm *current);

typedef struct {
    /* per char parsing */
    char *cur;
    unsigned int row;
    unsigned int col;

    REG pc;
    mot* mem;
    REG memsize;
    char *program;
    
    LblListElm *labels;
} Parser;

typedef enum {
    PM_REG,     // RX
    PM_IMM,     // #X
    PM_IND,     // [RX]
    PM_DIR,     // [X]
    //PM_DEP    // X[RY]   //TODO
} PMode;

LblListElm *lbllist_add(LblListElm *head, REG ptr, char *str, size_t len)
{
    LblListElm *newhead = malloc(sizeof(*newhead));

    newhead->next = head;
    newhead->pointer = ptr;
    newhead->str = malloc(len + 1);
    strcpy(newhead->str, str);

    return newhead;
}

LblListElm *lbllist_next(LblListElm *current)
{
    return current->next;
}

mode pseudomode_to_mode(PMode spmode, PMode dpmode)
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
        default:
            return REGISTER;
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
            return INDIRECT;
        }
        break;
    case PM_IMM:
        return IMMEDIATE;
    case PM_DIR:
        switch(spmode)
        {
        case PM_REG:
            return DIRREG;
        case PM_IMM:
            return DIRIMM;
        default:
            return DIRECT;
        }
        break;
    }
    return REGISTER;
}

bool sivm_parse_file(int* memsize, mot *mem[], char *file)
{
    bool ret;
    int size;
    FILE* f = fopen(file, "r");
    if(f == NULL) {
        perror("fopen");
        return false;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);
    
    char buffer[size + sizeof('\0')];

    fread(buffer, size + sizeof('\0'), 1, f);
    buffer[size] = '\0';
    ret = sivm_parse(memsize, mem, buffer);
    fclose(f);

    return ret;
}

bool iswhitespace(char c)
{
    return c == ' ' || c == '\t';
}

int ascii2base(char ascii, int base)
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
            if(*parser->cur == 'b' &&
               (*(parser->cur+1) == '0' || *(parser->cur+1) == '1'))
            {
                base = 2;
            }
            else
            {
                for(LblListElm* lbl = parser->labels; lbl;
                     lbl = lbllist_next(lbl))
                {
                    if(!strncmp(lbl->str, parser->cur, strlen(lbl->str)))
                    {
                        exit(1);
                    }
                }
            }
        }
        else
        {
            fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                    parser->row, parser->col, *parser->cur);
            return false;
        }
    }
    
    if(*parser->cur == '0' && *(parser->cur + 1) == 'x')
        base = 16;
    
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
    for(; iswhitespace(*parser->cur); parser->cur++,parser->col++)
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
        fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
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
        for(; iswhitespace(*parser->cur); parser->cur++,parser->col++)
        {}

        if(*parser->cur != ']')
        {
            fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                    parser->row, parser->col, *parser->cur);
            return false;
        }
    }

    return true;
}

bool parse_1attrib(Parser* parser, mot m[3], unsigned int *instrsize)
{
    // source
    int sdata, sreg;
    PMode spmode;

    // assert there is at least one whitespace before
    if(!iswhitespace(*parser->cur))
    {
        fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                parser->row, parser->col, *parser->cur);
        exit(1);
    }

    // read the source
    if(!parse_attrib(parser, &spmode, &sdata, &sreg))
    {
        fprintf(stderr, "parse_attrib returned false\n");
        return false;
    }
    if(spmode == PM_REG || spmode == PM_IND)
        m[0].codage.source = sreg;
    if(spmode == PM_IMM || spmode == PM_DIR)
        m[(*instrsize)++].brut = sdata;

    for(; iswhitespace(*parser->cur);
        parser->col++,parser->cur++)
    {}

    switch(spmode)
    {
    case PM_REG:
        m[0].codage.mode = REGISTER;
        break;
    case PM_IMM:
        m[0].codage.mode = IMMEDIATE;
        break;
    case PM_DIR:
        m[0].codage.mode = DIRECT;
        break;
    case PM_IND:
        m[0].codage.mode = INDIRECT;
        break;
    }

    return true;
}

bool parse_2attribs(Parser* parser, mot m[3], unsigned int *instrsize)
{
    // source
    int sdata, sreg;
    PMode spmode;

    // destination
    int ddata, dreg;
    PMode dpmode;

    // assert there is at least one whitespace before
    if(!iswhitespace(*parser->cur))
    {
        fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                parser->row, parser->col, *parser->cur);
        exit(1);
    }

    // read the destination
    if(!parse_attrib(parser, &dpmode, &ddata, &dreg))
    {
        fprintf(stderr, "parse_attrib return false\n");
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
        fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                parser->row, parser->col, *parser->cur);
        exit(1);
    }

    for(; iswhitespace(*parser->cur);
        parser->col++,parser->cur++)
    {}

    // read the source
    if(!parse_attrib(parser, &spmode, &sdata, &sreg))
    {
        return false;
        fprintf(stderr, "parse_attrib return false\n");
    }
    if(spmode == PM_REG || spmode == PM_IND)
        m[0].codage.source = sreg;
    if(spmode == PM_IMM || spmode == PM_DIR)
        m[(*instrsize)++].brut = sdata;

    for(; iswhitespace(*parser->cur);
        parser->col++,parser->cur++)
    {}

    m[0].codage.mode = pseudomode_to_mode(spmode, dpmode);

    return true;
}

bool parse_pass_line(Parser* parser, char *line)
{
    char instr[256];
    mot m[3];
    unsigned int instrsize;

    parser->cur = line;
    parser->col = 1;

    // skipy
    for(; iswhitespace(*parser->cur);
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

        // load SOURCE, DEST
        if(!strcmp(instr, "load"))
        {
            m[0].codage.codeop = LOAD;
            if(!parse_2attribs(parser, m, &instrsize))
                return false;
        }
        // store SOURCE, DEST
        else if(!strcmp(instr, "store"))
        {
            m[0].codage.codeop = STORE;
            if(!parse_2attribs(parser, m, &instrsize))
                return false;
        }
        // add SOURCE, DEST
        else if(!strcmp(instr, "add"))
        {
            m[0].codage.codeop = ADD;
            if(!parse_2attribs(parser, m, &instrsize))
                return false;
        }
        // sub SOURCE, DEST
        else if(!strcmp(instr, "sub"))
        {
            m[0].codage.codeop = SUB;
            if(!parse_2attribs(parser, m, &instrsize))
                return false;
        }
        // jmp SOURCE
        else if(!strcmp(instr, "jmp"))
        {
            m[0].codage.codeop = JMP;
            if(!parse_1attrib(parser, m, &instrsize))
                return false;
        }
        // jeq SOURCE
        else if(!strcmp(instr, "jeq"))
        {
            m[0].codage.codeop = JEQ;
            if(!parse_1attrib(parser, m, &instrsize))
                return false;
        }
        // call SOURCE
        else if(!strcmp(instr, "call"))
        {
            m[0].codage.codeop = CALL;
            if(!parse_1attrib(parser, m, &instrsize))
                return false;
        }
        // ret
        else if(!strcmp(instr, "ret"))
        {
            m[0].codage.codeop = RET;
        }
        // push SOURCE
        else if(!strcmp(instr, "push"))
        {
            m[0].codage.codeop = PUSH;
            if(!parse_1attrib(parser, m, &instrsize))
                return false;
        }
        // pop DEST
        else if(!strcmp(instr, "pop"))
        {
            m[0].codage.codeop = POP;
            if(!parse_1attrib(parser, m, &instrsize))
                return false;
        }
        // halt
        else if(!strcmp(instr, "halt"))
        {
            m[0].codage.codeop = HALT;
        }
        else
        {
            instrsize = 0;
        }
        
        // write instruction into the memory
        if(instrsize > 0)
        {
            if(!checkModes(m[0]))
            {
                fprintf(stderr,
                        "Unknown modes for instruction at line %d\n",
                        parser->row);
                // return false;
            }
            
            if(parser->mem)
            {
                // second pass
                for(int i = 0; i < instrsize; i++)
                    parser->mem[parser->pc++].brut = m[i].brut;
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
    char *save_ptr;
    char *line;
    char *program;
    char program_save[strlen(parser->program) + 1];
    strcpy(program_save, parser->program);
    
    parser->labels = NULL;
    parser->mem = NULL;
    parser->row = 1;
    parser->pc = 0;
    
    // read the buffer line by line
    for(program = program_save; ; parser->row++)
    {
        line = strtok_r(program, "\n\r", &save_ptr);
        if (line == NULL)
            break;
        program = NULL;
        
        printf("pass1: %s\n", line);

        char instr[256];
        
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
               // printf("added %s @ %.4X\n", instr, parser->pc);
            }
        }

        // we need to parse instructions, because we need to know the exact
        // addresse we are
        if(!parse_pass_line(parser, line))
        {
            fprintf(stderr, "parse_pass_line returned false\n");
            return false;
        }
    }
    
    parser->memsize = parser->pc;
    // allocate the memory correspind size needed to write the code
    parser->mem = malloc(parser->memsize * sizeof(parser->mem[0]));
    
    return true;
}

/**Parses a second time the source, to assemble
 *@param    parser      pointer to the Parser structure
 *@returns	false if any error occurs
 */
bool parse_second_pass(Parser* parser)
{
    char *save_ptr;
    char *line;
    char *program;
    char program_save[strlen(parser->program) + 1];
    strcpy(program_save, parser->program);
    
    parser->row = 1;
    parser->pc = 0;
    
    // read the buffer line by line
    for(program = program_save; ; parser->row++)
    {
        line = strtok_r(program, "\n\r", &save_ptr);
        if (line == NULL)
            break;
        program = NULL;

        parser->cur = line;
        parser->col = 1;
        printf("pass2: %s\n", line);
        
        if (!parse_pass_line(parser, line))
        {
            fprintf(stderr, "parse_pass_line returned false\n");
            return false;
        }
    }

    return true;
}

bool sivm_parse(int* memsize, mot *mem[], char *str)
{
    Parser parser;

    parser.program = str;
    
    if(!parse_first_pass(&parser))
    {
        fprintf(stderr, "parse_first_pass returned false\n");
        return false;
    }
    
    if(!parse_second_pass(&parser))
    {
        fprintf(stderr, "parse_second_pass returned false\n");
        return false;
    }

    // 
    *memsize = parser.memsize;
    *mem = parser.mem;

    return true;
}
