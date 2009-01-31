#include <stdlib.h>
#include <string.h>

#include "cmd_word.h"
#include "instructions.h"


/**Computes the destination and source adressing modes from a command word.
 *@param	w			the command word from which to compute adressing modes
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
			logm(LOG_FATAL_ERROR, "Invalid adressing mode");
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
	char *color = "";
	char buffer[2 * MAX_INSTR_PRINT_SIZE * sizeof(char)];
    buffer[0] = '\0';
	
	switch (m) {
		case REGISTER:
			if (ANSI_OUTPUT) color = "\e[36m";
			if (paramType == CMD_WORD_SOURCE_INDEX)
				sprintf(buffer, "R%d", w.codage.source);
			else if (paramType == CMD_WORD_DEST_INDEX)
				sprintf(buffer, "R%d", w.codage.dest);
			else {
				logm(LOG_WARNING, "Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)");
				if (ANSI_OUTPUT) color = "\e[41m";
				strcat(buffer, "R?");
			}
			break;
		case IMMEDIATE:
			if (ANSI_OUTPUT) color = "\e[32m";
			sprintf(buffer, "#%u", w.brut);
			break;
		case DIRECT:
			if (ANSI_OUTPUT) color = "\e[34m";
			sprintf(buffer, "[%u]", w.brut);
			break;
		case INDIRECT:
			if (ANSI_OUTPUT) color = "\e[33m";
			if (paramType == CMD_WORD_SOURCE_INDEX)
				sprintf(buffer, "[R%d]", w.codage.source);
			else if (paramType == CMD_WORD_DEST_INDEX)
				sprintf(buffer, "[R%d]", w.codage.dest);
			else {
				logm(LOG_WARNING, "Unable to determine parameter type\n(invalid paramType argument passed to appendParameter)");
				if (ANSI_OUTPUT) color = "\e[41m";
				strcat(buffer, "[R?]");
			}
			break;
		default:
			logm(LOG_WARNING, "Unable to determine parameter\n(invalid mode argument passed to appendParameter)");
			if (ANSI_OUTPUT) color = "\e[41m";
			strcat(buffer, "??");
			break;
	}
	
	char buffer2[sizeof(buffer)];
	strcpy(buffer2, buffer);
	sprintf(buffer, "%s%s%s%s", (paramType == CMD_WORD_SOURCE_INDEX ? "\t" : ""), color, buffer2, (ANSI_OUTPUT ? "\e[0m" : ""));

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
		logm(LOG_WARNING, "Disassembly could not finish properly because the program is not complete!\n\t(adressing mode asks for more words than given)");
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
	char *buffer = malloc((length + 2) * sizeof(char) * MAX_INSTR_PRINT_SIZE); //+2 for the legend lines
	buffer[0] = '\0'; //prevent useless characters cross-platform-wise
	if (ANSI_OUTPUT)
		strcat(buffer, "\e[35m");
	strcat(buffer, "L (PC)|\t\tInstr\tDest\tSource\n-----------------------------------------\n");
	if (ANSI_OUTPUT)
		strcat(buffer, "\e[0m");
		   
	int line = 1;
	for (int i = 0; i < length; i++)
	{
		char lineBuffer[MAX_INSTR_PRINT_SIZE];
		
		sprintf(lineBuffer, (ANSI_OUTPUT ? "\e[35m%.2d (%.2d)  |\t" : "%.2d (%.2d)  |\t"), line, i);
		if (ANSI_OUTPUT) strcat(lineBuffer, "\e[0m");
		
		strcat(buffer, lineBuffer);
		line++;

		int incr = disassemble_single_instruction(buffer, &words[i]);
		while (--incr) {
			if (! disassembler_increment_reading_pointer(length, &i))
				return strcat(buffer, "***end of program reached***");
		}

		strcat(buffer, "\n");
	}
	return buffer;
}

/**Disassemble one instruction only.
 *@param	buffer	the string to which append the disassembled instruction
 *@param	words	an array containing all needed words. You may put a much longer array if you will, only the number of words actually needed from the first word parsing will be read.
 *@returns	the number of words read by decoding this instruction
 */
int disassemble_single_instruction(char *buffer, const cmd_word words[])
{
	int read = 0;
	cmd_word currentWord = words[read];
	Instr instruction = getInstruction(currentWord);
	
	strcat(buffer, instruction.name);
	
	strcat(buffer, "\t");
	
	mode sourceMode, destMode;
	getModes(&currentWord, &destMode, &sourceMode);
	
	if (instruction.source || instruction.destination)
	{
		switch (currentWord.codage.mode) {
				//whole command is 3 words long
			case DIRIMM:
			case INDIMM:
				if (instruction.source && instruction.destination) //just for the sake of robustness, this should be forbidden in the parser anyway
				{
					read++;
				} else { //normally impossible to encounter
					logm(LOG_WARNING, "Disassembly encountered an incorrect adressing mode\n\t(adressing mode requiring 3 words for a command allowing 1 or less parameter)");
					return -1;
				}
				
				//whole command is 2 words long and the second word (first being the instruction one) is the source parameter
			case REGIMM:
			case REGDIR:
				if (instruction.destination)
					appendParameter(buffer, words[read], destMode, CMD_WORD_DEST_INDEX);
				

				read++;
				if (instruction.source)
					appendParameter(buffer, words[read], sourceMode, CMD_WORD_SOURCE_INDEX);
				
				break;
				
				//whole command is 2 words long and the second word (first being the instruction one) is the destination parameter
			case DIRREG:
			case INDREG:
				read++;
				
				if (instruction.destination)
					appendParameter(buffer, words[read], destMode, CMD_WORD_DEST_INDEX);
				if (instruction.source)
					appendParameter(buffer, currentWord, sourceMode, CMD_WORD_SOURCE_INDEX);
				break;
				
				//whole command is 1 word long
			case REGREG:
			case REGIND:
				if (instruction.destination)
					appendParameter(buffer, words[read], destMode, CMD_WORD_DEST_INDEX);
				if (instruction.source)
					appendParameter(buffer, words[read], sourceMode, CMD_WORD_SOURCE_INDEX);
				break;
				
			default:
				logm(LOG_FATAL_ERROR, "Invalid adressing mode");
				return -1;
		}
	}
	return read + 1;
}
//@}
