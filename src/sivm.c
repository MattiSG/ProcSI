#include "sivm.h"

#include "instructions.h"

/**@name	SIVM setup*/
//@{

/**Initializes an SIVM.
 *Inits PC, SP and SR registers to the values defined in the SIVM.h file.
 *Inits all memory and registers of the SIVM to 0.
 */
void sivm_new(SIVM *sivm)
{
    sivm->pc = PC_START;
    sivm->sp = SP_START;
    sivm->sr = SR_START;

    for (unsigned int i = 0; i < NREGS; i++)
        sivm->reg[i] = 0;
    for (unsigned int i = 0; i < MEMSIZE; i++)
        sivm->mem[i].brut = 0;
}

/**Loads the given program in the given SIVM.
 *@returns	false if the SIVM's memory is too small to load the whole program, true if the loading was successful.
 */
bool sivm_load(SIVM *sivm, int memsize, mot mem[memsize])
{
	if (memsize > MEMSIZE) return false;
	
    for (unsigned int i = 0; i < memsize; i++)
        sivm->mem[i] = mem[i];

    return true;
}
//@}


void increment_PC(SIVM *);

bool sivm_exec(SIVM *, mot *);

/**@name	SIVM instructions execution*/
//@{

/**Executes the next instruction in the given SIVM.
 *@see	sivm_exec
 *@returns	true if the instruction was correctly executed, false if the SIVM has to stop (whether on HALT instruction or because of a bad instruction).
 */
bool sivm_step(SIVM *sivm)
{
	increment_PC(sivm);
    mot *m = &sivm->mem[sivm->pc];

    /* stop the vm */
    if (m->codage.codeop == HALT)
        return false;
	
    return sivm_exec(sivm, m);
}

/**Handles PC incrementation for an SIVM.
 *Also checks for PC validity, which is why you shouldn't increment PC by hand.
 */
void increment_PC(SIVM *sivm)
{
	if (sivm->pc + 1 <= MEMSIZE)
		sivm->pc++;
	else
		quit("PC too high !");
}

/**Executes the given word in the given SIVM.
 *Checks for invalid adressing modes.
 *@returns	true if the instruction was correctly executed.
 */
bool sivm_exec(SIVM *sivm, mot *word)
{	
	Instr instr = getInstruction(word);
	mode destMode, srcMode;
	REG *dest;
	mot source;
	
	switch (word->codage.mode) {
		case REGREG:
			destMode = REGISTER;
			srcMode = REGISTER;
			break;
		case REGIMM:
			destMode = REGISTER;
			srcMode = IMMEDIATE;
			break;
		case REGDIR:
			destMode = REGISTER;
			srcMode = DIRECT;
			break;
		case REGIND:
			destMode = REGISTER;
			srcMode = INDIRECT;
			break;
		case DIRIMM:
			destMode = DIRECT;
			srcMode = IMMEDIATE;
			break;
		case DIRREG:
			destMode = DIRECT;
			srcMode = IMMEDIATE;
			break;
		case INDIMM:
			destMode = INDIRECT;
			srcMode = IMMEDIATE;
			break;
		case INDREG:
			destMode = INDIRECT;
			srcMode = REGISTER;
			break;
		default:
			quit("Invalid opcode");
	}
	
	switch (destMode) {
		case REGISTER:
			dest = &sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			quit("Cannot assign to an immediate value!");
			break;
		case DIRECT:
			increment_PC(sivm);
			dest = &(sivm->mem[sivm->pc].brut);
			break;
		case INDIRECT:
			dest = &(sivm->mem[sivm->reg[word->codage.dest]].brut);
			break;
		default:
			quit("Invalid destination adressing mode");
	}
	
	switch (srcMode) {
		case REGISTER:
			source = (mot) sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			increment_PC(sivm);
			source = sivm->mem[sivm->pc];
			break;
		case DIRECT:
			source = sivm->mem[word->codage.dest];
			break;
		case INDIRECT:
			source = sivm->mem[sivm->reg[word->codage.dest]];
			break;
		default:
			quit("Invalid source adressing mode");
	}
	
	bool result = instr.function(sivm, dest, source);
	
	if (result) sivm->sr = *dest;
	return result;
}
//@}


void sivm_status(SIVM *sivm)
{
    printf("==> Status <==\n");
    for (unsigned int i = 0; i < NREGS; ++i)
        printf("\tREG[%d] = %d\n", i, sivm->reg[i]);
}