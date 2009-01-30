#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/**@name	Display and logging settings
 *Defines the level of verbosity of the program and level of output formatting.
 *0 is the less verbose mode (displays fatal errors only).
 */
//@{
/**Activate colored output or not*/
#define ANSI_OUTPUT true
/**Maximum level of messages to be displayed to stderr*/
#define ERR_LOG_LEVEL 5
/**Maximum level of messages to be displayed to stdout*/
#define OUT_LOG_LEVEL 5
/**Level of message from which error is considered as fatal (exits)*/
#define FATAL_LEVEL 0
//@}

/**Read a line from the standard input
 *@param	str		pointer to the string
 *@param	length	size of the maximum input
 *@return
 */
bool readLine(char *str, size_t length);

/**Logs debugging messages.
 *@param	level	the priority level of the message ; maximum priority is 0, and should be used for fatal errors only.
 *@param	msg		the message to log
 *@see	ERR_LOG_LEVEL
 *@see	OUT_LOG_LEVEL
 */
void logm(char level, char *format, ...);

#endif /*UTIL_H*/
