#include "instructions.h"

#include "util.h"

/**@name	Instructions*/
//@{

/**Emulates the LOAD command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_load(SIVM *sivm, REG *dest, const mot source)
{
	logm("LOAD", 5);
	*dest = source.brut;
    return true;
}

/**Emulates the STR command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_store(SIVM *sivm, REG *dest, const mot source)
{
	logm("STR", 5);
	*dest = source.brut;
    return true;
}

/**Emulates the ADD command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_add(SIVM *sivm, REG *dest, const mot source)
{
	logm("ADD", 5);
	*dest += source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the SUB command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_sub(SIVM *sivm, REG *dest, const mot source)
{
	logm("SUB", 5);
	*dest -= source.brut;
	sivm->sr = *dest;
    return true;
}

/**Emulates the JMP command in the given SIVM.
 *This instruction takes only one parameter above the targeted SIVM, source, but keeps the dest argument for type compatibility.
 *<strong>WARNING</strong> the argument to use is <strong>source</strong> and not dest.
 *Argument validity will be checked.
 *@param	source	the adress at which the PC register of the given SIVM should be put at.
 *@return	true if the command was successful, false if the PC argument is out of memory bounds.
 */
bool instr_jmp(SIVM *sivm, REG *dest, const mot source)
{
	logm("JMP", 5);
	if (source.brut > MEMSIZE) {
		logm("Jumping too far!", 0);
		return false;
	}
	sivm->pc = source.brut;
    return true;
}

/**Emulates the MOV command in the given SIVM.
 *@return	true if the command was successful.
 */
bool instr_mov(SIVM *sivm, REG *dest, const mot source)
{
	logm("MOV", 5);
	*dest = source.brut;
	sivm->sr = *dest;
    return true;
}

/**Describes all available instructions.
 *This array has keys taken from the "instructions" enum, and its values describe the corresponding instructions, as an array containing :
 *<ol>
 *	<li>a pointer to the function emulating the instruction to execute</li>
 *	<li>a bitfield describing which adressing modes are legal for this instruction</li>
 *</ol>
 *The bitfield element is created from bit-to-bit OR operations between all the f_modes enum elements.
 *@see	instructions.h#enum instructions
 *@see	instructions.h#enum f_modes
 *@see	getInstruction
 */
Instr instructions[] = {
	[LOAD] = {instr_load, FM_REGDIR | FM_REGIMM | FM_REGIND},
	[STORE] = {instr_store, FM_DIRIMM | FM_DIRREG | FM_INDIMM | FM_INDREG},
	[ADD] = {instr_add, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND},
	[SUB] = {instr_sub, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND},
	[JMP] = {instr_jmp, FM_REGREG | FM_REGIMM | FM_REGDIR | FM_REGIND},
	[MOV] = {instr_mov, FM_REGREG | FM_REGIMM}
};

//@}


/**Tests a word for adressing modes legality.
 *The whole instruction word is tested, in order to determine whether the adressing modes used in it are legal for the instruction it contains.
 *@return	true if the adressing modes are legal.
 */
bool checkModes(mot *m)
{
    return getInstruction(m).modes & (1 << m->codage.mode);
}

/**Returns the instruction encoded in the given word.
 *This accessor is an interface to the private "instructions" array.
 *@see	instructions.h#Instr
 */
Instr getInstruction(mot *m)
{
	return instructions[m->codage.codeop];
}