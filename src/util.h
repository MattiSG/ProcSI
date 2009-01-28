#ifndef UTIL_H
#define UTIL_H

/**@name	Logging levels
 *Defines the level of verbosity of the program.
 *0 is the less verbose mode (displays fatal errors only).
 */
//@{
/**Maximum level of messages to be displayed to stderr*/
#define ERR_LOG_LEVEL 5
/**Maximum level of messages to be displayed to stdout*/
#define OUT_LOG_LEVEL 5
//@}

void quit(char *msg);

/**Logs debugging messages.
 *@param	msg		the message to log
 *@param	level	the priority level of the message ; maximum priority is 0, and should be used for fatal errors only.
 *@see	ERR_LOG_LEVEL
 *@see	OUT_LOG_LEVEL
 */
void logm(char* msg, char level);

#endif /*UTIL_H*/
