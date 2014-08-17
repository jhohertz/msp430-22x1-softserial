
#ifndef CLI_H_
#define CLI_H_

#include "uart.h"
#include "strings.h"
#include "cli_commands.h"

#define CMDBUFSIZE     20
#define PROMPT          "\n\r$ "

void cli_parser(char * , struct sCommandStruct *);
void cli_evaluate(void);
void cli_input(char);

#endif /*CLI_H_*/

