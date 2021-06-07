#include "datatypes.h"
#include "StringUtils.h"
#include "memops.h"

/*
Function Name: addToMemList
Algorithm: adds to end of linked list
Preconditon: given pointer to front of queue or null
Postcondition: creates new linked list or appends node
               to end of linked list
Exceptions: none
Notes: none
*/ 
memBlock *addToMemList( memBlock *localPtr, memBlock *newBlock)
{
   //check for local ptr assigned to null
   if(localPtr == NULL)
   {
      //allocate memory for new node
      localPtr = (memBlock *) malloc(sizeof(memBlock));
      //assign all values to newly created node
      localPtr->lowerBound = newBlock->lowerBound;
      localPtr->upperBound = newBlock->upperBound;

      //assign next node to null
      localPtr->nextAlloc = NULL;

      return localPtr;

   }

  //assume not at end of list
  localPtr->nextAlloc = addToMemList(localPtr->nextAlloc, newBlock);
  return localPtr;
   
}

/*
Function name: accessMemory
Algorithm: checks if memory request is valid and
           returns true if valid request and false
           if request is invalid
Precondition: given process and op code with request for memory
Postcondition: returns true upon successful access or false if 
               unsuccessful access
*/
Boolean accessMemory(ProcessControlBlock *currentProcess, 
                       OpCodeType *memRequest)
{
    //initialize variables
    memBlock *memAllocs = currentProcess->allocdMem;

    //iterate through allocated memory for given process
    while(memAllocs != NULL)
    {
      //check if request >= lower bound and <= upper boud of this mem block
      if(memRequest->intArg2 >= memAllocs->lowerBound && 
          (memRequest->intArg2 + memRequest->intArg3) <= memAllocs->upperBound)
      {
         //if yes, return true 
         return True;
      }
         //if not, move on to next allocated block
      memAllocs = memAllocs->nextAlloc;

     } 
     //end loop

   //return false upon loop end
   return False;
}

/*
Function name: allocateMemory
Algorithm: checks if requested memory allocation
           is valid, if yes, allocates memory,
           if not, returns error
Precondition: given process control block and op 
              code with memory allocation request
Postcondition: allocates memory or returns error
*/
Boolean allocateMemory(ProcessControlBlock **currentProcess,
                                    ProcessControlBlock *queueHead, 
                                    OpCodeType* memRequest,
                                    ConfigDataType *configPtr)
{
   //check if allocation request is valid
   if(verifyMemAlloc(*currentProcess, queueHead, memRequest,
                             configPtr) == True)
   {

      //if yes, add memory allocation to current PCB
      memBlock *newMem = (memBlock*)malloc(sizeof(memBlock));
      newMem->lowerBound = memRequest->intArg2;
      newMem->upperBound = (memRequest->intArg2 + memRequest->intArg3); 
      (*currentProcess)->allocdMem = addToMemList((*currentProcess)->allocdMem, 
                                                 newMem);
      //return true
      return True;
   }
   
   //if not, return false
   return False;
}

/*
Function Name: clearMemQueue
Algorithm: recursively iterates through process linked list,
           returns memory to OS from the bottom of the list upward
Precondition: linked list, with or without data
Postcondition: all node memory, if any, is returned to OS,
               return pointer (head) is set to nulll
Exceptions: none
Notes: none
*/
memBlock *clearMemQueue( memBlock *localPtr )
   { 
      //check for local pointer not set to null(list not empty)
      if( localPtr != NULL )
      {      
         //call recursive function with next pointer
          clearMemQueue( localPtr->nextAlloc );      
         //after recursive call, release memory to OS
            //function: free
         free( localPtr );   
         //set local pointer to null
         localPtr = NULL;
      }   
      //return null to calling function
      return NULL;
   }


/*
Function name: verifyMemAlloc
Algorithm: iterates through process control blocks, 
           checking if memory requested has not already been allocated
Precondition: given valid process queue and current process as well as op
              code with memory allocation request
Postcondition: returns true if memory has not yet been allocated and is 
               within limits of memory or false if memory has either already
               been allocated or is outside limits
*/
Boolean verifyMemAlloc(ProcessControlBlock *currentProcess,
                         ProcessControlBlock *queueHead,
                            OpCodeType *memRequest,
                             ConfigDataType *configPtr)
{
   //initialize variables
   ProcessControlBlock *tempPtr = queueHead;
   memBlock *memPtr;


   //check if base + offset > amt in config file
   if((memRequest->intArg2 + memRequest->intArg3) > configPtr->memAvailable)
   {
      //if so, return false
      return False;
   }
   //iterate through PCBs
   while(tempPtr != NULL)
   {
      memPtr = tempPtr->allocdMem;

      //iterate through memory allocations for current process
      while(memPtr != NULL)
      {
         //if requested allocation overlaps with already allocated memory, 
         //return false
         if((memRequest->intArg2 <= memPtr->upperBound) &&
             ((memRequest->intArg2 + memRequest->intArg3) >= memPtr->lowerBound))
         {
            return False;
         }
         memPtr = memPtr->nextAlloc;


      }

      tempPtr = tempPtr->nextNode;

   }
   //end loop
   

   //return true upon loop exit 
   return True;
}