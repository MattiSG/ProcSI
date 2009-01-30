#include <stdlib.h>
#include <string.h>

#include "cmd_word.h"
#include "instructions.h"


/**Computes the destination and source adressing modes from a command word.
 *@param	word	the command word from which to compute adressing modes
 *@param	sourceMode	pointer to the source mode variable to set
 *@param	destMode	pointer to the destination mode variable to set
 *@return	false if the given modes are illegal
 */
bool getModes(cmd_word *word, mode *destMode, mode *sourceMode)
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
			*sourceMode = REGISTER;
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


/**@name	Disassembly*/
//@{
/**Appends the given word's command name at the end of the given string.
 *@returns	pointer to the populated string
 */
char* appendName(char *string, const cmd_word w)
{
	return strcat(string, getInstruction(w).name);
}

/**Appends the given parameter at the end of the given string.
 *@param	string	the string to populate
 *@param	w		the word representing the parameter. If the mode makes the a
 *@returns	pointer to the populated string
 */
char* appendParameter(char *string, const cmd_word w, mode m)
{
	return strcat(string, getInstruction(w).name);
}

/**Returns a string with the given instructions in assembly form.
 *@param	length	length of the words array
 *@param	words	array of cmd_word to disassemble
 *@returns	a string representing words in assembly language
 */
char* disassemble(size_t length, const cmd_word words[])
{
	char *buffer = malloc(length * sizeof(char) * MAX_INSTR_PRINT_SIZE);
	for (int i = 0; i < length; i++)
	{
		cmd_word currentWord = words[length];
		appendName(buffer, currentWord);
		strcpy(buffer, "\t");
		switch (currentWord.codage.mode) {
			//whole command is 1 word long
			case REGREG:
			case REGIND:
				break;
				
			//whole command is 2 words long
			case REGIMM:
			case REGDIR:
			case DIRREG:
			case INDREG:
				break;
				
			//whole command is 3 words long
			case DIRIMM:
			case INDIMM:
				break;
			default:
				logm("Invalid adressing mode", 0);
				return false;
		}
	}
	return buffer;
}
//@}