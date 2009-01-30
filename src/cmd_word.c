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
bool getModes(cmd_word *w, mode *destMode, mode *sourceMode)
{
	switch (w->codage.mode) {
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
char* appendInstructionName(char *string, const cmd_word w)
{
	return strcat(string, getInstruction(w).name);
}

/**Appends the given source parameter at the end of the given string.
 *@param	string	the string to populate
 *@param	w		the word containing the parameter's value. If the mode makes the parameter one that should be given in a whole word (ie. IMMEDIATE or DIRECT), this parameter w has to be the full word, not the one containing the instruction. On the contrary, if the mode makes the parameter one that is included in the command word, you have to hand the command word to this function.
 *@param	m		the adressing mode for the given parameter
 *@param	paramType	one of the constants defined in sivm.h defining the indexes of source/dest parameters in a command word. Has to be set if parameter w is the command word itself, will be ignored if w is a rough adress.
 *@returns	pointer to the populated string
 */
char* appendParameter(char *string, const cmd_word w, mode m, int paramType)
{
	char buffer[MAX_INSTR_PRINT_SIZE];
	switch (m) {
		case REGISTER:
			if (paramType == CMD_WORD_SOURCE_INDEX)
				sprintf(buffer, "R%d", w.codage.source);
			else if (paramType == CMD_WORD_DEST_INDEX)
				sprintf(buffer, "R%d", w.codage.dest);
			else {
				logm("Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)", 2);
				strcat(buffer, "R?");
			}
			break;
		case IMMEDIATE:
			sprintf(buffer, "#%u", w.brut);
			break;
		case DIRECT:
			sprintf(buffer, "[%u]", w.brut);
			break;
		case INDIRECT:
			if (paramType == CMD_WORD_SOURCE_INDEX)
				sprintf(buffer, "[R%d]", w.codage.source);
			else if (paramType == CMD_WORD_DEST_INDEX)
				sprintf(buffer, "[R%d]", w.codage.dest);
			else {
				logm("Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)", 2);
				strcat(buffer, "[R?]");
			}
			break;
		default:
			logm("Unable to determine parameter\n(invalid mode argument passed to appendParameter)", 2);
			strcat(buffer, "??");
			break;
	}
	return strcat(string, buffer);
}

/**Returns a string with the given instructions in assembly form.
 *@param	length	length of the words array
 *@param	words	array of cmd_word to disassemble
 *@returns	a string representing words in assembly language
 */
char* disassemble(int length, const cmd_word words[])
{
	char *buffer = malloc(length * sizeof(char) * MAX_INSTR_PRINT_SIZE);
	for (int i = 0; i < length; i++)
	{
		cmd_word currentWord = words[i];
		appendInstructionName(buffer, currentWord);
		
		strcat(buffer, "\t");
		
		mode sourceMode, destMode;
		getModes(&currentWord, &destMode, &sourceMode);

		switch (currentWord.codage.mode) {
			//whole command is 3 words long
			case DIRIMM:
			case INDIMM:
				if (i + 1 < length)
					i++;
				else {
					logm("Disassembly could not finish properly because the program is not complete!\n(adressing mode asks for more words than given)", 2);
					return strcat(buffer, "***end of program reached***");
				}

			//whole command is 2 words long
			case REGIMM:
			case REGDIR:
			case DIRREG:
			case INDREG:
				appendParameter(buffer, words[i], destMode, CMD_WORD_DEST_INDEX);
				if (i + 1 < length)
					i++;
				else {
					logm("Disassembly could not finish properly because the program is not complete!\n(adressing mode asks for more words than given)", 2);
					return strcat(buffer, "***end of program reached***");
				}
				break;
				
			//whole command is 1 word long
			case REGREG:
			case REGIND:
				appendParameter(buffer, words[i], destMode, CMD_WORD_DEST_INDEX);
				break;
								
			default:
				logm("Invalid adressing mode", 0);
				return false;
		}
		strcat(buffer, "\t");
		appendParameter(buffer, words[i], sourceMode, CMD_WORD_SOURCE_INDEX);
		strcat(buffer, "\n");
	}
	return buffer;
}
//@}