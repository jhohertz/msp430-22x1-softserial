
#ifndef CLI_COMMANDS_H_
#define CLI_COMMANDS_H_

#define COMMANDLISTSIZE 8

typedef struct sCommandStruct {char * commandString;
                               void (*function)(void);
                              } tCommandStruct;

extern const tCommandStruct commandList[COMMANDLISTSIZE];

void led1On(void);
void led2On(void);
void led1Off(void);
void led2Off(void);
void led1Toggle(void);
void led2Toggle(void);
void cliHelp(void);
void reset(void);
#endif /*CLI_COMMANDS_H_*/

