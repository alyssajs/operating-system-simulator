//pre-compiler directive
#ifndef OUTPUTOPS_H
#define OUTPUTOPS_H

//header files
#include "datatypes.h"
#include "simtimer.h"

//function headers
outputNode *addToOutputList( outputNode *localPtr, char *newLine, ConfigDataType *configPtr );
outputNode *clearOutputList( outputNode *localPtr );
void writeToFile(ConfigDataType *configPtr, outputNode *outputHead);




#endif //OUTPUTOPS_H
