
#include "strings.h"

/*  
    Compares a string to a base or master string.
    Returns:
    0 for identical,
    1 for string two identical until white space in place of NULL,
    -1 for not identical
*/
int stringCompare(char * masterString, char * comparisonString)
{
    int rtn = -1;

    while (*masterString == *comparisonString)
    {
        if (*masterString == '\0' )
        {
            rtn = 0;
            break;
        }

        else if ( *(masterString+1) == '\0' && (*(comparisonString+1) == ' ' ||
                  *(comparisonString+1) == '\n' || *(comparisonString+1) == '\r'))
        {
            rtn = 1;
            break;
        }

        masterString++;
        comparisonString++;
    }
    return rtn;
}

/*
    Zeros out an area of memory
*/
void memClear(char * dataPtr, unsigned int numberOfBytes)
{
    int ctr;
    for(ctr=0; ctr<numberOfBytes; ctr++) {
        dataPtr[ctr] = 0x00;
    }
}

