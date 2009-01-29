#include "parser.h"

typedef struct {
    unsigned int row;
    unsigned int col;
    char* cur;
} Parser;

typedef enum {
    PM_REG,     // RX
    PM_IMM,     // #X
    PM_IND,     // [RX]
    PM_DIR,     // [X]
    //PM_DEP    // X[RY]   //TODO
} PMode;

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

bool sivm_parse_file(int memsize, mot mem[memsize], char *file)
{
    int size;
    char* buffer;
    FILE* f = fopen(file, "r");
    if(f == NULL) {
        perror("fopen");
        return false;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);
    buffer = malloc(size + sizeof('\0'));
    fread(buffer, size + sizeof('\0'), 1, f);
    buffer[size] = '\0';
    sivm_parse(memsize, mem, buffer);
    free(buffer);
    fclose(f);

    return true;
}

bool iswhitespace(char c)
{
    return c == ' ' || c == '\t';
}

/**Parses a number
 *@param    parser      a pointer to the Parser structure
 *@param    value       a pointer to an output int
 *@param    modifiable  tells if the numbers base can be modifier (0xY, Yh)
 *@returns	false if no number can be read
 */
bool parse_number(Parser* parser, int* value, bool modifiable)
{
    // should here be expecting a number (even a register number)
    if(*parser->cur < '0' || *parser->cur > '9')
    {
        fprintf(stderr, "Unexpected token at %d:%d : `%c'\n",
                parser->row, parser->col, *parser->cur);
        exit(1);
    }

    *value = 0;
    
    // read the number
    for(; *parser->cur && *parser->cur >= '0' && *parser->cur <= '9';
        parser->col++,parser->cur++)
    {
        *value = *value * 10 + (*parser->cur - '0');
    }

    if(modifiable)
    {
        // if the number is hexa
        if(*parser->cur == 'h')
        {
            fprintf(stderr, "not yet implemented");
            exit(1);
        }
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
    
    // it is a bird, is it a plane ? no, it's a register
    if(*parser->cur == 'R' || *parser->cur == 'r')
    {
        isregister = true;
        parser->col++;
        parser->cur++;
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
        return false;
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
        return false;
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

bool sivm_parse(int memsize, mot mem[memsize], char *str)
{
    REG pc = 0;
    Parser parser;
    char *save_ptr;
    char *line = str;

    parser.row = 0;
    parser.col = 0;
    
    // read the buffer line by line
    for((line = strtok_r(str, "\n\r", &save_ptr)); line;
        (line = strtok_r(NULL, "\n\r", &save_ptr)), parser.row++)
    {
        char instr[256];
        mot m[3];
        unsigned int instrsize;
        
        parser.cur = line;
        parser.col = 1;

        // skipy
        for(; iswhitespace(*parser.cur);
            parser.col++, parser.cur++)
        {}

        // read the instructions name
        if(sscanf(parser.cur, "%s", instr) == 1)
        {
            instrsize = 1;
            parser.cur += strlen(instr);
            parser.col += strlen(instr);

            // add SOURCE, DEST
            if(!strcmp(instr, "add"))
            {
                m[0].codage.codeop = ADD;
                if(!parse_2attribs(&parser, m, &instrsize))
                    return false;
            }
            // halt
            else if (!strcmp(instr, "halt"))
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
                printf("s: %d\n", instrsize);
                if(!checkModes(m[0]))
                {
                    fprintf(stderr,
                            "Unknown modes for instruction at line %d\n",
                            parser.row);
                    return false;
                }
                for(int i = 0; i < instrsize; i++)
                    mem[pc++].brut = m[i].brut;
            }
        }
    }

    return true;
}
