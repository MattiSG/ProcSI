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
	
	if (SP_START + SP_INCR > MEMSIZE || SP_START + SP_INCR <= 0)
		logm("Stack init and incrementation are not in the same way, VM will crash at first push.", 0);
	
    for (unsigned int i = 0; i < NREGS; i++)
        sivm->reg[i] = 0;
    for (unsigned int i = 0; i < MEMSIZE; i++)
        sivm->mem[i].brut = 0;
	
	logm("VM successfully initialized.", 2);
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


bool increment_PC(SIVM *);

bool sivm_exec(SIVM *, mot *);


/**@name	Consistency checks*/
//@{
/**Checks whether the given index is legal for access to the memory.
 *@returns	true if the access is legal.
 */
bool checkMemoryAccess(REG index)
{
	if (index > MEMSIZE) {
		quit("Invalid memory access (too high!)");
		return false;
	}
	return true;
}

/**Checks whether the given index is legal for access to a register.
 *@returns	true if the access is legal.
 */
bool checkRegisterAccess(unsigned index)
{
	if (index >= NREGS) {
		quit("Invalid register access (too high!)");
		return false;
	}
	return true;
}
//@}


/**@name	SIVM instructions execution*/
//@{

/**Executes the next instruction in the given SIVM.
 *@see	sivm_exec
 *@see	increment_PC
 *@returns	true if the instruction was correctly executed, false if the SIVM has to stop (whether on HALT instruction or because of a bad instruction).
 */
bool sivm_step(SIVM *sivm)
{
	checkMemoryAccess(sivm->pc);
    mot *m = &sivm->mem[sivm->pc];

    /* stop the vm */
    if (m->codage.codeop == HALT) {
		logm("HALT instruction encountered, stopping VM.", 3);
        return false;
	}
	
    if (! sivm_exec(sivm, m)) return false;

	if (! increment_PC(sivm))
		quit("sivm_step:\nPC too high !");
	
	return true;
}

/**Handles PC incrementation for an SIVM.
 *Also checks for PC validity, which is why you shouldn't increment PC by hand.
 *@returns	false if PC can't be incremented anymore (ie current PC >= MEMSIZE)
 */
bool increment_PC(SIVM *sivm)
{
	
	if (sivm->pc < MEMSIZE) {
		sivm->pc++;
		return true;
	}
	logm("PC too high !", 0);
	return false;
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
			checkRegisterAccess(word->codage.dest);
			dest = &sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			quit("Cannot assign to an immediate value!");
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->pc);
			dest = &(sivm->mem[sivm->pc].brut);
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			dest = &(sivm->mem[sivm->reg[word->codage.dest]].brut);
			break;
		default:
			quit("Invalid destination adressing mode");
	}

	switch (srcMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			source = (mot) sivm->reg[word->codage.source];
			break;
		case IMMEDIATE:
			increment_PC(sivm);
			source = sivm->mem[sivm->pc];
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->pc);
			source = (mot) sivm->mem[sivm->pc].brut;
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			source = sivm->mem[sivm->reg[word->codage.source]];
			break;
		default:
			quit("Invalid source adressing mode");
	}

	if (instr.function(sivm, dest, source)) {
		logm("Instruction successful", 5);
		return true;
	} else {
		logm("Instruction unsuccessful", 5);
		return false;
	}
}
//@}


/**Prints the given SIVM's registers values.
 */
void sivm_status(SIVM *sivm)
{
    printf("==> Status <==\n");
	printf("\tPC = %d\n", sivm->pc);
	printf("\tSR = %d\n", sivm->sr);
 	printf("\tSP = %d\n", sivm->sp);
    for (unsigned int i = 0; i < NREGS; ++i)
        printf("\tREG[%d] = %d\n", i, sivm->reg[i]);
}