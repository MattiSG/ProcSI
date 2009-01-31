#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "sivm.h"

/**@name	Display and logging settings
 *Defines the level of verbosity of the program and level of output formatting.
 *0 is the less verbose mode (displays fatal errors only).
 */
//@{
/**Activate colored output or not*/
#define ANSI_OUTPUT true
/**Level for messages informing user about important good steps (ie. init successful).
 *These won't be affected by LOG_LEVEL.
 */
#define LOG_STEP 100

#define LOG_FATAL_ERROR 0
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_INFO 3
#define LOG_DEBUG 4
/**Level of message from which error is considered as fatal (exits)*/
#define FATAL_LEVEL 0
/**Maximum level of messages to be displayed to stderr*/
#define ERR_LOG_LEVEL 5
/**Maximum level of messages to be displayed to stdout*/
#define OUT_LOG_LEVEL 5
//@}

/**Read a line from the standard input
 *@param	str		pointer to the string
 *@param	length	size of the maximum input
 *@return
 */
bool readLine(char *str, size_t length);

/**Logs debugging messages.
 *@param	level	the priority level of the message ; maximum priority is 0, and should be used for fatal errors only.
 *@param	format, ...		the message to log, see printf
 *@see	ERR_LOG_LEVEL
 *@see	OUT_LOG_LEVEL
 */
void logm(char level, char *format, ...);

/**Try to make a last-moment recovery from an invalid value.
 *@param	val	pointer to the value to possibly modify
 *@param	format, ...		the message to log, see printf
 *@returns	true if the user wanted to modify the value, false if not
 */
bool superRecover(REG *val, char *format, ...);
#endif /*UTIL_H*/
