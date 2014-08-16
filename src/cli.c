
#include "cli.h"

char cmdBuffer[CMDBUFSIZE];
unsigned int cmdBufferIndex = 0;

// add char to cmd buffer, evaluate after
void cli_input(char in) {
    // overflow
    if(cmdBufferIndex >= CMDBUFSIZE) {
        // panic and reset
        cmdBufferIndex = 0;
        memClear((char*)cmdBuffer, sizeof(cmdBuffer));
        uartPrint(PROMPT);
    } else  {
        // add and evaluate
        cmdBuffer[cmdBufferIndex++] = in;
        cli_evaluate();
    }

}

void cli_evaluate(void) {
    // skip if empty
    if(cmdBufferIndex >= 0) {
        // overflow
        if(cmdBufferIndex >= CMDBUFSIZE) {
            cmdBufferIndex = 0;
            memClear((char*)cmdBuffer, sizeof(cmdBuffer));
            uartPrint(PROMPT);
        }
        /* If last char received was a newline, parse */
        else if(cmdBuffer[cmdBufferIndex -1] == '\n' ||
                cmdBuffer[cmdBufferIndex -1] == '\r')
        {
            cmdBuffer[cmdBufferIndex - 1] = '\0';
            cli_parser((char *)cmdBuffer, (tCommandStruct *)commandList);
            // reset
            cmdBufferIndex = 0;
            memClear((char *)cmdBuffer, CMDBUFSIZE );
            uartPrint(PROMPT);
        }
    }
}

/* Function runs through all commands in argument of tCommandStruct comparing with
 * stringToParse. If a command matches, the corresponding function pointer is called
 */
void cli_parser(char * stringToParse, tCommandStruct * commandStructPtr)
{
    int ctr;

    for(ctr = 0; ctr < COMMANDLISTSIZE; ctr++)
    {
        if (stringCompare(commandStructPtr[ctr].commandString, stringToParse) >= 0)
        {
            commandStructPtr[ctr].function();
        }
    }
}

