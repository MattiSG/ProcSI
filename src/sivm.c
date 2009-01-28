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
		logm("Invalid memory access (too high!)", 0);
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
		logm("Invalid register access (too high!)", 0);
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
	
	if (checkMemoryAccess(sivm->pc)) {
		sivm->pc++;
		return true;
	}
	logm("PC too high !", 0);
	return false;
}

/**Computes the destination and source adressing modes from a command word.
 *@param	word	the command word from which to compute adressing modes
 *@param	destMode	pointer to the destination mode variable
 *@param	sourceMode	pointer to the source mode variable
 *@return	false if the given modes are illegal
 */
bool getModes(mot *word, mode *destMode, mode *sourceMode)
{
	switch (word->codage.mode) {
		case REGREG:
			*destMode = REGISTER;
			*sourceMode = REGISTER;
			break;
		case REGIMM:
			*destMode = REGISTER;
			*sourceMode = IMMEDIATE;
			break;
		case REGDIR:
			*destMode = REGISTER;
			*sourceMode = DIRECT;
			break;
		case REGIND:
			*destMode = REGISTER;
			*sourceMode = INDIRECT;
			break;
		case DIRIMM:
			*destMode = DIRECT;
			*sourceMode = IMMEDIATE;
			break;
		case DIRREG:
			*destMode = DIRECT;
			*sourceMode = IMMEDIATE;
			break;
		case INDIMM:
			*destMode = INDIRECT;
			*sourceMode = IMMEDIATE;
			break;
		case INDREG:
			*destMode = INDIRECT;
			*sourceMode = REGISTER;
			break;
		default:
			logm("Invalid adressing mode", 0);
			return false;
	}
	return true;
}

/**Computes the destination and source from a command word.
 *@param	sivm	the VM in which to get the parameters
 *@param	word	the command word from which to compute parameters
 *@param	dest	pointer to the destination parameter
 *@param	source	pointer to the source parameter
 *@return	false if the given modes are illegal
 *
bool getParameters(SIVM *sivm, mot *word, REG **dest, mot *source)
{
	mode destMode, srcMode;
	getModes(word, &destMode, &srcMode);
	
	switch (destMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			*dest = &sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			logm("Invalid adressing mode: cannot assign to an immediate value!", 0);
			return false;
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->pc);
			*dest = &(sivm->mem[sivm->pc].brut);
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			*dest = &(sivm->mem[sivm->reg[word->codage.dest]].brut);
			break;
		default:
			logm("Invalid destination adressing mode", 0);
			return false;
	}
	printf("dest = %d", **dest);
	
	switch (srcMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			source = (mot*) &sivm->reg[word->codage.source];
			break;
		case IMMEDIATE:
			increment_PC(sivm);
			source = &sivm->mem[sivm->pc];
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->pc);
			source = (mot*) &sivm->mem[sivm->pc].brut;
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			source = &sivm->mem[sivm->reg[word->codage.source]];
			break;
		default:
			logm("Invalid source adressing mode", 0);
			return false;
	}
	return true;
}
*/
/**Executes the given word in the given SIVM.
 *Checks for invalid adressing modes.
 *@returns	true if the instruction was correctly executed.
 */
bool sivm_exec(SIVM *sivm, mot *word)
{	
	Instr instr = getInstruction(*word);
	REG *dest;
	mot source;
	
//	getParameters(sivm, word, &dest, &source);
	
	mode destMode, srcMode;
	getModes(word, &destMode, &srcMode);

	switch (destMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			dest = &sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			logm("Invalid adressing mode: cannot assign to an immediate value!", 0);
			return false;
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
			logm("Invalid destination adressing mode", 0);
			return false;
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
			logm("Invalid source adressing mode", 0);
			return false;
	}
	
	logm(instr.name, 5);
	if (instr.function(sivm, dest, source)) {
		logm("Instruction successful", 5);
		return true;
	} else {
		logm("Instruction unsuccessful", 5);
		return false;
	}
}
//@}


/**@name	SIVM status inquiry*/
//@{
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

/**Returns the given SIVM's current instruction.*/
Instr sivm_get_instruction(SIVM *sivm)
{
	checkMemoryAccess(sivm->pc);
	return getInstruction(sivm->mem[sivm->pc]);
}

/**Fills a string with the given SIVM's current instruction in a human-readable form.*/
void sivm_get_instruction_string(SIVM *sivm, char *buffer)
{
	Instr instr = sivm_get_instruction(sivm);
	strcpy(buffer, instr.name);
	strcpy(buffer, "\t");
	
}
//@}