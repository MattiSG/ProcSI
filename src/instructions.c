#include "instructions.h"

#include "util.h"

/**@name	Instructions*/
//@{

/**Emulates the LOAD command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_load(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest = source.brut;
    return true;
}

/**Emulates the STR command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_store(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest = source.brut;
    return true;
}

/**Emulates the MOV command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_mov(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest = source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the ADD command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_add(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest += source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the SUB command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_sub(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest -= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the AND command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_and(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest &= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the OR command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_or(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest |= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the DEC command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_dec(SIVM *sivm, REG *dest, const cmd_word source)
{
	--(*dest);
	sivm->sr = *dest;
    return true;
}

/**Emulates the SHL command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_shl(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest <<= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the SHR command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_shr(SIVM *sivm, REG *dest, const cmd_word source)
{
	*dest >>= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the CMP command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_cmp(SIVM *sivm, REG *dest, const cmd_word source)
{
	sivm->sr = (*dest - source.brut);
    return true;
}

/**Emulates the JMP command in the given SIVM.
 *This instruction takes only one parameter above the targeted SIVM, source, but keeps the dest argument for type compatibility.
 *<strong>WARNING</strong> the argument to use is <strong>source</strong> and not dest.
 *Argument validity will be checked.
 *@param	source	the adress at which the PC register of the given SIVM should be put at.
 *@return	true if the command was successful, false if the PC argument is out of memory bounds.
 */
bool instr_jmp(SIVM *sivm, REG *dest, const cmd_word source)
{
	if (source.brut > MEMSIZE) {
		logm(0, "Jumping too far!");
		return false;
	}
	sivm->pc = source.brut - 1;
    return true;
}

/**Emulates the JEQ command in the given SIVM.
 *@see	instr_jmp
 */
bool instr_jeq(SIVM *sivm, REG *dest, const cmd_word source)
{
	if (sivm->sr == 0)
		return instr_jmp(sivm, dest, source);
    return false;
}

/**Emulates the PUSH command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_push(SIVM *sivm, REG *dest, const cmd_word source)
{
	REG newSp = sivm->sp + SP_INCR;
	checkMemoryAccess(&sivm->sp);
	checkMemoryAccess(&newSp);
	sivm->mem[sivm->sp] = source;
	sivm->sp = newSp;
	return true;
}

/**Emulates the POP command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_pop(SIVM *sivm, REG *dest, const cmd_word source)
{
	REG newSp = sivm->sp - SP_INCR;
	checkMemoryAccess(&newSp);
	*dest = sivm->mem[newSp].brut;
	sivm->sp = newSp;
	return true;
}

/**Emulates the CALL command in the given SIVM.
 *@see	instr_push
 *@see	instr_jmp
 */
bool instr_call(SIVM *sivm, REG *dest, const cmd_word source)
{
	if (! instr_push(sivm, dest, (cmd_word) sivm->pc))
		return false;
	
	for (int i = 0; i < NREGS; i++)
		if (i < PARAM_REGS_START || i >= PARAM_REGS_END) //ignore reserved registers
			if (! instr_push(sivm, dest, (cmd_word) sivm->reg[i]))
				return false;
	
	return instr_jmp(sivm, dest, source);
}

/**Emulates the RET command in the given SIVM.
 *@see	instr_pop
 *@see	instr_jmp
 */
bool instr_ret(SIVM *sivm, REG *dest, const cmd_word source)
{
	for (int i = NREGS - 1; i >= 0; i--)
		if (i < PARAM_REGS_START || i >= PARAM_REGS_END) //ignore reserved registers
			if (! instr_pop(sivm, &sivm->reg[i], source))
				return false;
	
	return instr_pop(sivm, &sivm->pc, source);
}

/**Emulates the HALT command in the given SIVM.
 *@return	false, in order to stop the VM.
 */
bool instr_halt(SIVM *sivm, REG *dest, const cmd_word source)
{
	logm(3, "HALT instruction encountered.");
    return false;
}

/**Describes all available instructions.
 *This array has keys taken from the "instructions" enum, and its values describe the corresponding instructions, as an array containing :
 *<ol>
 *	<li>a pointer to the function emulating the instruction to execute</li>
 *	<li>a bitfield describing which adressing modes are legal for this instruction</li>
 *</ol>
 *The bitfield element is created from bit-to-bit OR operations between all the f_modes enum elements. An instruction whose nargs == 0 should have its bitfield set to 0x0 to get standard behavior from checkModes.
 *@see	instructions.h#enum instructions
 *@see	instructions.h#enum f_modes
 *@see	getInstruction
 */
Instr instructions[] = {
	[LOAD]	= {instr_load,	true,	true,	FM_REGDIR | FM_REGIMM | FM_REGIND,				"LOAD"},
	[STORE]	= {instr_store,	true,	true,	FM_DIRIMM | FM_DIRREG | FM_INDIMM | FM_INDREG,	"STORE"},
	[MOV]	= {instr_mov,	true,	true,	FM_REGREG | FM_REGIMM,							"MOV"},
			
	[ADD]	= {instr_add,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"ADD"},
	[SUB]	= {instr_sub,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"SUB"},
	[AND]	= {instr_and,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"AND"},
	[OR]	= {instr_and,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"OR"},
	[SHL]	= {instr_shl,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"SHL"},
	[SHR]	= {instr_shr,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"SHR"},
//	[CMP]	= {instr_cmp,	true,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND | DIRIMM | DIRREG | INDIMM | INDREG,	"CMP"}, //we had no more room for extra instructions
			
	[JMP]	= {instr_jmp,	false,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"JMP"},
	[JEQ]	= {instr_jeq,	false,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"JEQ"},
			
	[PUSH]	= {instr_push,	false,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"PUSH"},
	[POP]	= {instr_pop,	true,	false,	FM_REGREG,	"POP"},
			
	[CALL]	= {instr_call,	false,	true,	FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND,	"CALL"},
	[RET]	= {instr_ret,	false,	false,	0x0,	"RET"},

	[HALT]	= {instr_halt,	false,	false,	0x0,	"HALT"},
};

//@}


/**Tests a word for adressing modes legality.
 *The whole instruction word is tested, in order to determine whether the adressing modes used in it are legal for the instruction it contains.
 *@return	true if the adressing modes are legal, false in the opposite case or if the command word takes no argument.
 */
bool checkModes(const cmd_word m)
{
    return getInstruction(m).modes & (1 << m.codage.mode);
}

/**Returns the instruction encoded in the given word.
 *This accessor is an interface to the private "instructions" array.
 *@see	instructions.h#Instr
 */
Instr getInstruction(const cmd_word m)
{
	return instructions[m.codage.codeop];
}
