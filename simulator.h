//pre-compiler directive
#ifndef SIMULATOR_H
#define SIMULATOR_H

//header files
#include "datatypes.h"
#include "simtimer.h"
#include "outputops.h"
#include "memops.h"
#include <pthread.h> 

//types
typedef enum {CHECK_FOR_INTERRUPTS, 
              SIGNAL_INTERRUPT, 
              CLEAR_ALL_INTERRUPTS} interruptCtrlCode;

//function headers
ioData *addToInterruptQueue( ioData *localPtr, ioData *newIoCmd );
ioData *dequeue( ioData *headPtr, ioData *toRemove);
Boolean manageInterrupts(int interruptCtrlCode, ioData *ioReturnData,  
                           ioData **interruptHead, 
                           ProcessControlBlock **queueHead, 
                           outputNode **outputHead, 
                           ConfigDataType *configPtr);
outputNode *processLine( outputNode *headPtr, 
                         ConfigDataType *configPtr,
                        ProcessControlBlock **currentProcess,
                        ProcessControlBlock *processQueueHead,
                        ioData **interruptHead );
void * runIO(void * inputData);
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );




#endif //SIMULATOR_H
