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
			logm(0, "Invalid adressing mode");
			return false;
	}
	return true;
}


/**@name	Disassembly*/
//@{
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
				logm(2, "Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)");
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
				logm(2, "Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)");
				strcat(buffer, "[R?]");
			}
			break;
		default:
			logm(2, "Unable to determine parameter\n(invalid mode argument passed to appendParameter)");
			strcat(buffer, "??");
			break;
	}
	
	if (paramType == CMD_WORD_DEST_INDEX)
		strcat(buffer, "\t");
	
	return strcat(string, buffer);
}

/**Increment the disassembler pointer securely.
 *Displays an error if the limit is reached too early.
 */
bool disassembler_increment_reading_pointer(const int max, int *reader)
{
	if ((*reader) + 1 < max)
	{
		(*reader)++;
		return true;
	}
	else {
		logm(2, "Disassembly could not finish properly because the program is not complete!\n\t(adressing mode asks for more words than given)");
		return false;
	}
}

/**Returns a string with the given instructions in assembly form.
 *@param	length	length of the words array
 *@param	words	array of cmd_word to disassemble
 *@returns	a string representing words in assembly language
 */
char* disassemble(int length, const cmd_word words[])
{
	char *buffer = malloc((length + 1) * sizeof(char) * MAX_INSTR_PRINT_SIZE); //+1 for the first line
    buffer[0] = '\0';
	strcat(buffer, "Line|\tInstr\tDest\tSource\n---------------------------------\n");
		   
	int line = 1;
	for (int i = 0; i < length; i++)
	{
		char lineBuffer[MAX_INSTR_PRINT_SIZE];
		sprintf(lineBuffer, "%d|\t", line);
		strcat(buffer, lineBuffer);
		line++;
		
		cmd_word currentWord = words[i];
		Instr instruction = getInstruction(currentWord);
		
		strcat(buffer, instruction.name);
		strcat(buffer, "\t");
		
		mode sourceMode, destMode;
		getModes(&currentWord, &destMode, &sourceMode);
		
		if (instruction.nargs > 0)
		{
			switch (currentWord.codage.mode) {
				//whole command is 3 words long
				case DIRIMM:
				case INDIMM:
					if (instruction.nargs > 1) //just for the sake of robustness, this should be forbidden in the parser anyway
					{
						if (! disassembler_increment_reading_pointer(length, &i))
							return strcat(buffer, "***end of program reached***");
					} else { //normally impossible to encounter
						logm(2, "Disassembly encountered an incorrect adressing mode\n\t(adressing mode requiring 3 words for a command allowing 1 or less parameter)");
						return strcat(buffer, "***incorrect number of parameters***");
					}
					
				//whole command is 2 words long and the second word (first being the instruction one) is the source parameter
				case REGIMM:
				case REGDIR:
					if (instruction.nargs > 1)
						appendParameter(buffer, words[i], destMode, CMD_WORD_DEST_INDEX);
					
					if (! disassembler_increment_reading_pointer(length, &i))
						return strcat(buffer, "***end of program reached***");
					
					appendParameter(buffer, words[i], sourceMode, CMD_WORD_SOURCE_INDEX);
					
					break;
					
				//whole command is 2 words long and the second word (first being the instruction one) is the destination parameter
				case DIRREG:
				case INDREG:
					if (! disassembler_increment_reading_pointer(length, &i))
						return strcat(buffer, "***end of program reached***");
					
					if (instruction.nargs > 1)
						appendParameter(buffer, words[i], destMode, CMD_WORD_DEST_INDEX);
					
					appendParameter(buffer, currentWord, sourceMode, CMD_WORD_SOURCE_INDEX);
					break;
					
				//whole command is 1 word long
				case REGREG:
				case REGIND:
					if (instruction.nargs > 1)
						appendParameter(buffer, words[i], destMode, CMD_WORD_DEST_INDEX);
					
					appendParameter(buffer, words[i], sourceMode, CMD_WORD_SOURCE_INDEX);
					break;
									
				default:
					logm(0, "Invalid adressing mode");
					return false;
			}
			strcat(buffer, "\t");
		}
		strcat(buffer, "\n");
	}
	return buffer;
}
//@}
