#include "sivm.h"

#include "instructions.h"
#include "cmd_word.h"

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
		logm(1, "Stack init and incrementation are not in the same way, VM will crash at first PUSH.");
	
	if (PARAM_REGS_END > NREGS || PARAM_REGS_START > NREGS || PARAM_REGS_END < PARAM_REGS_START)
		logm(1, "Reserved argument registers have illegal values, VM will crash at first CALL or RET.");
	
    for (unsigned int i = 0; i < NREGS; i++)
        sivm->reg[i] = 0;
    for (unsigned int i = 0; i < MEMSIZE; i++)
        sivm->mem[i].brut = 0;
	
	logm(2, "VM successfully initialized.");
}

/**Loads the given program in the given SIVM.
 *@returns	false if the SIVM's memory is too small to load the whole program, true if the loading was successful.
 */
bool sivm_load(SIVM *sivm, int memsize, cmd_word mem[memsize])
{
	if (memsize > MEMSIZE) return false;
	
    for (unsigned int i = 0; i < memsize; i++)
        sivm->mem[i] = mem[i];

    return true;
}
//@}


bool increment_PC(SIVM *);

bool sivm_exec(SIVM *, cmd_word *);


/**@name	Consistency checks*/
//@{
/**Checks whether the given index is legal for access to the memory.
 *@returns	true if the access is legal.
 */
bool checkMemoryAccess(REG index)
{
	if (index > MEMSIZE) {
		logm(0, "Invalid memory access: ", index);
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
		logm(0, "Invalid register access: ", index);
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
    cmd_word *m = &sivm->mem[sivm->pc];

    /* stop the vm */
    if (m->codage.codeop == HALT) {
		logm(3, "HALT instruction encountered, stopping VM.");
        return false;
	}
	
    if (! sivm_exec(sivm, m)) return false;

	return increment_PC(sivm);
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
	logm(0, "PC too high !");
	return false;
}

/**Computes the destination from a command word.
 *<strong>WARNING</strong>: updates PC if necessary
 *@param	sivm	the VM in which to get the parameters
 *@param	word	the command word from which to compute parameters
 *@param	dest	pointer to the pointer to the destination parameter
 *@return	a pointer to the destination operand, possibly null if the mode encoded in the given word is illegal
 */
REG* getDestinationParameter(SIVM *sivm, cmd_word *word)
{
	mode destMode, srcMode;
	getModes(word, &destMode, &srcMode);
	
	switch (destMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			return &sivm->reg[word->codage.dest];
			break;
		case IMMEDIATE:
			logm(0, "Invalid adressing mode: destination parameter can't be an immediate value!");
			return NULL;
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->mem[sivm->pc].brut);
			return &(sivm->mem[sivm->mem[sivm->pc].brut].brut);
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			return &(sivm->mem[sivm->reg[word->codage.dest]].brut);
			break;
		default:
			logm(0, "Invalid destination adressing mode");
			return NULL;
	}
}

/**Computes the source from a command word.
 *<strong>WARNING</strong>: updates PC if necessary
 *@param	sivm	the VM in which to get the parameters
 *@param	word	the command word from which to compute parameters
 *@param	source	pointer to the source parameter
 *@return	the value of the source operand
 */
cmd_word getSourceParameter(SIVM *sivm, cmd_word *word)
{
	mode destMode, srcMode;
	getModes(word, &destMode, &srcMode);
	
	switch (srcMode) {
		case REGISTER:
			checkRegisterAccess(word->codage.dest);
			return (cmd_word) sivm->reg[word->codage.source];
			break;
		case IMMEDIATE:
			increment_PC(sivm);
			return sivm->mem[sivm->pc];
			break;
		case DIRECT:
			increment_PC(sivm);
			checkMemoryAccess(sivm->mem[sivm->pc].brut);
			return (cmd_word) sivm->mem[sivm->mem[sivm->pc].brut].brut;
			break;
		case INDIRECT:
			checkMemoryAccess(sivm->reg[word->codage.dest]);
			return sivm->mem[sivm->reg[word->codage.source]];
			break;
		default:
			logm(0, "Invalid source adressing mode");
			return (cmd_word) {.codage = { HALT }}; //Don't test for this value to detect errors, because it might also be a valid integer value
	}
}

/**Executes the given word in the given SIVM.
 *Checks for invalid adressing modes.
 *<strong>WARNING</strong>: may update PC through calls to getSourceParameter and getDestinationParameter.
 *@see	getSourceParameter
 *@see	getDestinationParameter
 *@returns	true if the instruction was correctly executed.
 */
bool sivm_exec(SIVM *sivm, cmd_word *word)
{	
	Instr instr = getInstruction(*word);
	cmd_word source = getSourceParameter(sivm, word);
	REG *dest = getDestinationParameter(sivm, word);
	
	if (instr.function(sivm, dest, source)) {
		logm(5, "Instruction successful");
		return true;
	} else {
		logm(1, "Instruction unsuccessful");
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
//@}
