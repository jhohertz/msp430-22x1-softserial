#include "config.h"
#include "cli.h"

const tCommandStruct commandList[COMMANDLISTSIZE]= {{"led one on",&(led1On)},
                                                    {"led two on",&(led2On)},
                                                    {"led one off",&(led1Off)},
                                                    {"led two off",&(led2Off)},
                                                    {"led one toggle",&(led1Toggle)},
                                                    {"led two toggle",&(led2Toggle)},
                                                    {"help", &(cliHelp)},
                                                    {"reset", &(reset)}
                                                    };

void led1On(void)
{
    P1OUT |= RED_LED;
}

void led2On(void)
{
    P1OUT |= GRN_LED;
    uartPrint("TWO\n\r");
}

void led1Off(void)
{
    P1OUT &= ~RED_LED;
}

void led2Off(void)
{
    P1OUT &= ~GRN_LED;
}

void led1Toggle(void)
{
    P1OUT ^= RED_LED;
}

void led2Toggle(void)
{
    P1OUT ^= GRN_LED;
}

/* Resets mcu by writing to Watchdog Register without specifying password. */
void reset(void)
{
    WDTCTL = 0x00;
}

void cliHelp(void)
{
    int ctr;
    uartPrint("Options:\n\r");

    for(ctr=0;ctr<COMMANDLISTSIZE-1;ctr++)
    {
        uartPrint(commandList[ctr].commandString);
        uartPrint("\n\r");
    }
}

