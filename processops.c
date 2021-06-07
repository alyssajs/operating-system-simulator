#include "processops.h"
#include "simulator.h"
#include "configops.h"
#include "memops.h"
#include "simtimer.h"

/*
Function Name: addToQueue
Algorithm: adds to end of queue
Precondition: given pointer to front of queue or null
Postcondition: creates new linked list or appends PCB to end 
               of linked list
Exceptions: none
Notes: none
*/
ProcessControlBlock *addToQueue( ProcessControlBlock *localPtr, ProcessControlBlock *newPCB,
                                   ConfigDataType *configPtr, outputNode **outputHead)
{
   char outputLine[MAX_STR_LEN];

   //check for local pointer assigned to null
   if(localPtr == NULL )
   {
      //access memory for new node
      localPtr = (ProcessControlBlock *) malloc(sizeof(ProcessControlBlock));
      //assign all values to newly created node
      localPtr->processID = newPCB->processID;
      localPtr->processState = READY;
      localPtr->allocdMem = newPCB->allocdMem;
      localPtr->cpuCyclesLeft = newPCB->cpuCyclesLeft;
    
      //report state change
      char timeBuffer[STD_STR_LEN];
      double currentTime = accessTimer(LAP_TIMER, timeBuffer);
      char intBuffer[STD_STR_LEN];
      sprintf(outputLine, "%f", currentTime);
      concatenateString(outputLine, ",  OS: Process ");
      sprintf(intBuffer, "%d", (newPCB)->processID);
 
      concatenateString(outputLine, intBuffer);
      concatenateString(outputLine, " ");
      concatenateString(outputLine, "set from NEW to READY");


      (*outputHead) = addToOutputList(*outputHead, outputLine, configPtr);

  
      localPtr->nextOpCommand = newPCB->nextOpCommand;
      localPtr->timeRemaining = newPCB->timeRemaining;

      //assign next pointer to null
      localPtr->nextNode = NULL;


      //return local pointer
      return localPtr;

   }
   //assume end of list not found yet
   //assign recursive function to current's next link
   localPtr->nextNode = addToQueue(localPtr->nextNode, newPCB, configPtr, outputHead);

   //return current local pointer
   return localPtr;
  
}

/*
Function Name: calcTimeRemaining 
Algorithm: iterates through op commands, calculates time,
           and adds to total time
Precondition: given pointer to process control block
Postcondition: returns amount of time remaining in that process
Exceptions: none
Notes: none
*/
int calcTimeRemaining(ProcessControlBlock *pcb, ConfigDataType *configDataPtr)
{
   //initialize variables
   OpCodeType *currentLoc = pcb->nextOpCommand;
   int totalTime = 0;
   int thisCommandTime = 0;

   //while pointer not at app end
   while(currentLoc != NULL && (!(compareString( currentLoc->command, "app" ) == STR_EQ
           && compareString(currentLoc->strArg1, "end" ) == STR_EQ)))
   {      
      thisCommandTime = getCommandTime( currentLoc, configDataPtr );
      totalTime = totalTime + thisCommandTime;

      //go to next op command
      currentLoc = currentLoc->nextNode;
   }

   //return total time
   return totalTime;

     
}

/*
Function Name: clearProcessQueue
Algorithm: recursively iterates through process linked list,
           returns memory to OS from the bottom of the list upward
Precondition: linked list, with or without data
Postcondition: all node memory, if any, is returned to OS,
               return pointer (head) is set to nulll
Exceptions: none
Notes: none
*/
ProcessControlBlock *clearProcessQueue( ProcessControlBlock *localPtr )
   { 
      //check for local pointer not set to null(list not empty)
      if( localPtr != NULL )
      {      
         //call recursive function with next pointer
          clearProcessQueue( localPtr->nextNode );      
         //after recursive call, release memory to OS
            //function: free
         localPtr->allocdMem = clearMemQueue(localPtr->allocdMem);
         free( localPtr );   
         //set local pointer to null
         localPtr = NULL;
      }   
      //return null to calling function
      return NULL;
   }





/*
Function Name: createPCB
Algorithm: takes meta data pointer and sets nextOpCode pointer
           and process state, and sets each op command to correct process
Precondition: given pointer to meta data list at "app start" command and 
               process id
Postcondition: returns pointer to new PCB with next op code and process state
               set as parameter, and returns incremented process ID
               to reflect the creation of a new process
Exceptions:none
Notes: none
*/
ProcessControlBlock *createPCB( OpCodeType *metaDataPointer,
                    int processID, ConfigDataType *configPtr)
{
   //allocate memory for new PCB
   ProcessControlBlock *newPcbPointer = 
          (ProcessControlBlock *) malloc(sizeof(ProcessControlBlock));
   //set process state
   newPcbPointer->processState = NEW;
   //set next op command pointer for this process to current location in meta data
   newPcbPointer->nextOpCommand = metaDataPointer;

   //set process ID
   newPcbPointer->processID = processID;

   //set allocated memory to null
   newPcbPointer->allocdMem = NULL;

   //set cycles left to 0
   newPcbPointer->cpuCyclesLeft = getNextCpuCycles(newPcbPointer);
 

   //loop until metaDataPointer at app end
   while(!(compareString( metaDataPointer->command, "app") == STR_EQ
               && compareString(metaDataPointer->strArg1, "end") == STR_EQ))
        {
          //assign process id of op code to processID
          metaDataPointer->pid = processID;
          
          //get next op command
          metaDataPointer = metaDataPointer->nextNode;
        }
        //end loop

   //set app end pid to proper pid
   metaDataPointer->pid = processID;

   //calculate time remaining
   newPcbPointer->timeRemaining = calcTimeRemaining(newPcbPointer, configPtr);


   //return incremented process id
   return newPcbPointer;

} 

/*
Function Name: createProcessQueue
Algorithm: iterates through meta data list,
           creating process control blocks and 
           adding to process queue depending on scheduling code
Precondition: given pointer to head of meta data list and scheduling code
Postcondition: returns pointer to head of queue of process control blocks
Exceptions: none
Notes: none
*/
ProcessControlBlock *createProcessQueue(OpCodeType *metaDataPointer, ConfigDataType *configPtr, outputNode **outputHead)
{
   //initialize variables
      ProcessControlBlock *headPtr = NULL;
      ProcessControlBlock *newPcbPtr = NULL;

      //initialize number of created processes
      int numProcesses = 0;

   //allocate memory for the temporary data structure
   //loop until at sys end
   while( !(compareString( metaDataPointer->command, "sys" ) == STR_EQ
           && compareString(metaDataPointer->strArg1, "end" ) == STR_EQ))
   {
      //check if app start
      if( compareString( metaDataPointer->command, "app" ) == STR_EQ
               && compareString( metaDataPointer->strArg1, "start") == STR_EQ )
      {
         //create PCB
         newPcbPtr = createPCB( metaDataPointer, numProcesses, configPtr );
         //add PCB to queue
         headPtr = addToQueue( headPtr, newPcbPtr, configPtr, outputHead );

   
         //increment number of processes
         numProcesses++;

      }
      //go to next command
       metaDataPointer = metaDataPointer->nextNode;

   }
   //end loop

   //return head of process queue
   return headPtr;
}

/*
Function Name: getNextCpuBurst
Algorithm: beginning at current instruction, iterates until instruction reaches
           CPU-using instruction
Precondition: Given ready process control block
Postcondition: Returns time that next CPU-using instruction will run for
Exceptions: none
Notes: none
*/
int getNextCpuBurst( ProcessControlBlock *currentProcess, ConfigDataType *configPtr )
{
   //initialize temporaray instruction pointer
   OpCodeType *tempPtr = currentProcess->nextOpCommand;
   int nextCpuBurst = 0;
 
   //iterate until we hit a cpu command or app end
   while(compareString(tempPtr->command, "cpu") != STR_EQ
            &&!(compareString(tempPtr->command, "app") == STR_EQ
                 &&compareString(tempPtr->strArg1, "end") == STR_EQ))
   {
      tempPtr = tempPtr->nextNode; 
   }

   //calculate amount of time it will take for that command to run
   nextCpuBurst = getCommandTime( tempPtr, configPtr );


   return nextCpuBurst;
}

int getNextCpuCycles( ProcessControlBlock *currentProcess )
{
   int numCycles = 0;
   Boolean cpuCmdFound = False;

   //check for next cpu command after current command
   OpCodeType *tempPtr = (currentProcess->nextOpCommand)->nextNode;
   while(tempPtr != NULL && cpuCmdFound == False &&
            compareString(tempPtr->command, "app") != STR_EQ)
   {
      if(compareString(tempPtr->command, "cpu") == STR_EQ)
      {
         cpuCmdFound=True;
         numCycles = tempPtr->intArg2;
      }
      tempPtr = tempPtr->nextNode;
   }
   return numCycles;
}

/*
Gets next process according to scheduling code stored in configPtr
Returns next process as parameter in returnedProcess
Returns PROCESS_AVAILABLE, ALL_PROCESSES_WAITING,
or ALL_PROCESSES_EXITING via integer
*/
int getNextProcess( ProcessControlBlock* headPtr,
                                     ProcessControlBlock* currentProcess,
                                     ProcessControlBlock **returnedProcess,
                                     ConfigDataType *configPtr )
{
   //check for current process null (meaning beginning of loop) 
   if(currentProcess == NULL)
   {
      currentProcess = headPtr;

      if(configPtr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE 
             || configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE
             || configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
      {
         *returnedProcess = currentProcess;
         (*returnedProcess)->processState = RUNNING;
         return PROCESS_AVAILABLE;
      }
   }
   
   int numWaiting = 0;

   int numExiting = 0;

   int totalProcesses = 0; 

   ProcessControlBlock *tempPtr = headPtr;
   while(tempPtr != NULL)
   {
      if(tempPtr->processState == WAITING)
      {
         numWaiting++;
      }
      if(tempPtr->processState == EXITING)
      {
         numExiting++;
      }
      totalProcesses++;
      tempPtr = tempPtr->nextNode;

   }

   //return all waiting if there's at least one waiting process
   //and all non-waiting processes are done (exiting)
   if(numWaiting > 0 && (numWaiting+numExiting)==totalProcesses)
   {
      return ALL_PROCESSES_WAITING;
   }

   if(numExiting==totalProcesses)
   {
      return ALL_PROCESSES_EXITING;
   }
 
   //initalize next process to null
   ProcessControlBlock *nextProcess = NULL;

   //check for FCFS
   if(configPtr->cpuSchedCode == CPU_SCHED_FCFS_N_CODE 
         || configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE)
   {
      nextProcess = headPtr;
      //if FCFS and not at beginning, return next ready process
      while(nextProcess != NULL && nextProcess->processState != READY)
      {
         nextProcess = nextProcess->nextNode;
      }
   }

  //check for RR
  if(configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
  {
     //loop back to beginning if already at the end
     if(currentProcess->nextNode == NULL)
     {
        nextProcess = headPtr;
     }
     //otherwise, start at next process
     else
     {
        nextProcess = currentProcess->nextNode;
     }
     while( nextProcess->processState != READY)
     {
        //move forward
        nextProcess = nextProcess->nextNode;
        //if we reached end, move back to beginning 
        if(nextProcess == NULL)
        {
           nextProcess = headPtr;
        }
     }
  }


   //check for SJF-N or SRTF-P
   if(configPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE || configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
   {
      //get first ready process if there is one
      nextProcess = headPtr;
      while(nextProcess != NULL && nextProcess->processState != READY)
      {
        nextProcess = nextProcess->nextNode;        
      }
      //make sure we got a process 
      if(nextProcess != NULL)
      {
   
         int minRunTime = nextProcess->timeRemaining;
         ProcessControlBlock *processToCompare = nextProcess->nextNode; 
         int currentRunTime;
         //iterate through processes
         while(processToCompare != NULL)
         {
           //check if current process is ready
           if(processToCompare->processState == READY)
           {

             currentRunTime = processToCompare->timeRemaining;
             //check if this process's next burst is less than currently chosen process
             if(currentRunTime < minRunTime)
             {
                //if yes, set chosen process to this process  
                minRunTime = currentRunTime;
                nextProcess = processToCompare;
             }
            //get another process to check

           }
            processToCompare = processToCompare->nextNode;
         }


      }
          
   } 
   //end loop	
   *returnedProcess = nextProcess;
   if((*returnedProcess)!=NULL)
   {
      (*returnedProcess)->processState = RUNNING;
   }
  
   //return next process
   return PROCESS_AVAILABLE;

}


