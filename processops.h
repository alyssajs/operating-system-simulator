//Pre-compiler directive
#ifndef PROCESSOPS_H
#define PROCESSOPS_H

#include "metadataops.h"

//process states
typedef enum { NEW,
               READY,
               RUNNING,
               WAITING,
               EXITING } processStates;

typedef enum { PROCESS_AVAILABLE,
               ALL_PROCESSES_WAITING,
               ALL_PROCESSES_EXITING } processListStatusCodes;

//method headers
ProcessControlBlock *addToQueue( ProcessControlBlock *localPtr, ProcessControlBlock *newPCB,
                                   ConfigDataType *configPtr, outputNode **outputHead);
int calcTimeRemaining(ProcessControlBlock *pcb, ConfigDataType *configDataPtr);
ProcessControlBlock *clearProcessQueue( ProcessControlBlock *localPtr );
ProcessControlBlock *createPCB( OpCodeType *metaDataPointer, int processID, ConfigDataType *configPtr);
ProcessControlBlock *createProcessQueue(OpCodeType *metaDataPointer, ConfigDataType *configPtr, outputNode **outputHead);
int getNextCpuBurst( ProcessControlBlock *currentProcess, ConfigDataType *configPtr );
int getNextCpuCycles( ProcessControlBlock *currentProcess);
int getNextProcess( ProcessControlBlock* headPtr,
                                     ProcessControlBlock* currentProcess,
                                     ProcessControlBlock **returnedProcess,
                                     ConfigDataType *configPtr );
char *getOutput( OpCodeType opCommand );



#endif //PROCESSOPS_H