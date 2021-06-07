//Pre-compiler directive
#ifndef MEMOPS_H
#define MEMOPS_H

#include "datatypes.h"
#include "StringUtils.h"

Boolean accessMemory(ProcessControlBlock *currentProcess, 
                       OpCodeType *memRequest);
memBlock *addToMemList( memBlock *localPtr, memBlock *newBlock);
Boolean allocateMemory(ProcessControlBlock **currentProcess,
                                    ProcessControlBlock *queueHead, 
                                          OpCodeType* memRequest,
                                     ConfigDataType *configPtr);
memBlock *clearMemQueue( memBlock *localPtr );
Boolean verifyMemAlloc(ProcessControlBlock *currentProcess,
                         ProcessControlBlock *queueHead,
                         OpCodeType *memRequest,
                         ConfigDataType *configPtr);

#endif //MEMOPS_H
