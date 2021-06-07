#include "simulator.h"
#include "processops.h"
#include "simtimer.h"
#include "datatypes.h"
#include "configops.h"
#include "outputops.h"
#include "memops.h"
#include <pthread.h> 



/*
Function Name: runSim
Algorithm: master driver for simulator operations;
           conducts OS simulation with varying scheduling strategies
           and varying numbers of processes
Precondition: given head pointer to config data and meta data
Postcondition: simulation is provided, file output is provided as configured
Exceptions: none
Notes: none
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
   {
      //initialize variables
      ProcessControlBlock *currentProcess = NULL;
      ProcessControlBlock *queueHead = NULL;
      outputNode *outputHead = NULL;
      int processStatus;
      char timeBuffer[STD_STR_LEN];
      OpCodeType *currentOpCmd = metaDataMstrPtr;
      ioData *interruptQueueHead = NULL;
      char output[MAX_STR_LEN];
      char intBuffer[STD_STR_LEN];
      double currentTime;
      Boolean preemptive;
      Boolean idlingReported = False; 
      int prevProcessID;
      if(configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE
           || configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE
           || configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
      {
            preemptive = True;
      }
     else
     {
        preemptive = False;
     } 
      //begin timer
      accessTimer(ZERO_TIMER, timeBuffer);


      //report simulator start 
      printf("Simulator Run\n");
      printf("-------------\n");

     //report system start
      currentTime = accessTimer(LAP_TIMER, timeBuffer);
      sprintf(output, "%f", currentTime);
      concatenateString(output, ",  OS: System start");
      outputHead = addToOutputList(outputHead, output, configPtr);

      //create process queue
      queueHead = createProcessQueue(metaDataMstrPtr, configPtr, &outputHead);

 

      //get first process
      processStatus = getNextProcess(queueHead, currentProcess, &currentProcess, configPtr); 
      //report first process choice
      currentTime = accessTimer(LAP_TIMER, timeBuffer);
      sprintf(output, "%f", currentTime);
      concatenateString(output, ",  OS: Process ");
      sprintf(intBuffer, "%d", currentProcess->processID);
      concatenateString(output, intBuffer);
      concatenateString(output, " selected with ");
      sprintf(intBuffer, "%d", currentProcess->timeRemaining);
      concatenateString(output, intBuffer);
      concatenateString(output, " ms remaining");
      outputHead = addToOutputList(outputHead, output, configPtr);

      //loop until all processes are exiting
      while(processStatus != ALL_PROCESSES_EXITING)
      {
         //check if we need to get another process
         if(currentProcess->processState != RUNNING)
         {

            prevProcessID = currentProcess->processID;
            //get next process
            do
            {
               processStatus = getNextProcess(queueHead, 
                      currentProcess, &currentProcess, configPtr);
               if(processStatus == ALL_PROCESSES_WAITING && idlingReported== False)
               {
                  currentTime = accessTimer(LAP_TIMER, timeBuffer);
                  sprintf(output, "%f", currentTime);
                  concatenateString(output, 
                                  ",  OS: CPU idle, all active processes waiting");  
                  outputHead = addToOutputList(outputHead, output, configPtr);           
                 

                  idlingReported=True;
               } 

               if(preemptive == True)
               {
                  manageInterrupts(CLEAR_ALL_INTERRUPTS, NULL, 
                                     &interruptQueueHead, &queueHead,
                                     &outputHead, configPtr);
               }
            }while(processStatus == ALL_PROCESSES_WAITING);
            //if cpu was idling, report that CPU stopped idling
            if(idlingReported == True)
            {
               currentTime = accessTimer(LAP_TIMER, timeBuffer); 
               sprintf(output, "%f", currentTime);
               concatenateString(output, 
                              ",  OS: CPU interrupt, end idle");

               outputHead = addToOutputList(outputHead, output, configPtr);           
         
               //reset idlingReported for next time CPU is idle
               idlingReported = False;

            }
            //if we got a new process than our previous, report change
            if(processStatus == PROCESS_AVAILABLE && 
                  prevProcessID != currentProcess->processID)
            {  
               currentTime = accessTimer(LAP_TIMER, timeBuffer);
               sprintf(output, "%f", currentTime);
               concatenateString(output, ",  OS: Process ");
               sprintf(intBuffer, "%d", currentProcess->processID);
               concatenateString(output, intBuffer);
               concatenateString(output, " ");
               //report process state modification to running
               //(done in getNextProcess)
               concatenateString(output, "selected");	
  
               sprintf(intBuffer, "%d", currentProcess->timeRemaining);
               concatenateString(output, " with ");
               concatenateString(output, intBuffer);
               concatenateString(output, " ms remaining");
               outputHead = addToOutputList(outputHead, output, configPtr);           
     
            }
                
         } 
         //process current operation
         outputHead = processLine(outputHead, configPtr, &currentProcess, queueHead,
                                   &interruptQueueHead);
         //check for and resolve interrupts if preemptive
         if(preemptive == True)
         {
            manageInterrupts(CLEAR_ALL_INTERRUPTS, NULL, &interruptQueueHead, &queueHead,
                                     &outputHead, configPtr);
         }
         
         //move process's next op cmd ptr fwd if we have a valid process
         // that's still running
         if(currentProcess != NULL && currentProcess->processState == RUNNING)
         {
            currentProcess->nextOpCommand = (currentProcess->nextOpCommand)->nextNode;
         }

      }
      //end loop

      //report system end
      currentTime = accessTimer(LAP_TIMER, timeBuffer);
      sprintf(output, "%f", currentTime);
      concatenateString(output, ",  OS: System stop ");

      outputHead = addToOutputList(outputHead, output, configPtr);


      //end timer
      accessTimer(STOP_TIMER, timeBuffer);
      
      //write operations to file if log to was both or file
      if(configPtr->logToCode == LOGTO_BOTH_CODE || configPtr->logToCode == LOGTO_FILE_CODE)
      {
         writeToFile(configPtr, outputHead);
      }
      //clear memory
      free(currentOpCmd);

      //free process queue
      queueHead = clearProcessQueue(queueHead);

      free(currentProcess);


      //free output queue

      outputHead = clearOutputList(outputHead);

   }

ioData *addToInterruptQueue( ioData *localPtr, ioData *newIoCmd )
{
   //check for local ptr assigned to null
   if(localPtr == NULL)
   {
      //allocate memory for new node
      localPtr = (ioData *) malloc(sizeof(ioData));
      //assign all values to newly created node
      copyString(localPtr->command, newIoCmd->command);
      localPtr->endTime = newIoCmd->endTime;
      localPtr->processID = newIoCmd->processID;
      //assign next node to null
      localPtr->nextNode = NULL;
      //assign reported value
      localPtr->reported = newIoCmd->reported;

      return localPtr;

   }

  //assume not at end of list
  localPtr->nextNode = addToInterruptQueue(localPtr->nextNode, newIoCmd);
  return localPtr;
   
}

/*
Removes given node from queue
*/
ioData *dequeue( ioData *current, ioData *toRemove)
{
   //check if we are at the end
   if(current == NULL)
   {
      return NULL;
   }

   //check if this is the node to remove
   if(compareString(toRemove->command, current->command) == STR_EQ
         && (toRemove->processID == current->processID))
   {

      ioData *tempNext = current->nextNode;
      free(current);
      current = NULL;


      return tempNext;
   }
   
   //go to next queue entry
   current->nextNode = dequeue(current->nextNode, toRemove);

   return current;
   
}

/*
If interruptCtrlCode is SIGNAL_INTERRUPT, adds ioDataReturnData to interruptQueue
If interruptCtrCode is CHECK_FOR_INTERRUPTS, looks for finished, unreported 
   interrupts and returns true if found
If interruptCtrlCode is CLEAR_ALL_INTERRUPTS, checks for finished interrupts,
   reports unreported interrupts, and clears them from the queue
Returns true is there were unreported, finished interrupts and false otherwise
   if CHECK_FOR_INTERRUPTS was code, if not returns false for other ctrl codes
*/
Boolean manageInterrupts(int interruptCtrlCode, ioData *ioReturnData, ioData **interruptHead, 
                           ProcessControlBlock **queueHead, outputNode **outputHead,
                           ConfigDataType *configPtr)
{
   ioData *tempPtr = *interruptHead;
   double currentTime;
   char timeBuffer[STD_STR_LEN];
   currentTime = accessTimer(LAP_TIMER, timeBuffer);
   char output[MAX_STR_LEN];
   char intBuffer[STD_STR_LEN];




   if(interruptCtrlCode == SIGNAL_INTERRUPT)
   {
      //add returned IO output to queue
         *interruptHead = addToInterruptQueue(*interruptHead, ioReturnData);

   }

   else if(interruptCtrlCode == CHECK_FOR_INTERRUPTS)
   {
      //interate through interrupt queue
      while(tempPtr != NULL)
      { 
         //if interrupt is done and not reported, report new interrupt
         if(tempPtr->endTime > currentTime)
         {

           if(tempPtr->reported == False)
           {
              currentTime = accessTimer(LAP_TIMER, timeBuffer);
              sprintf(output, "%f", currentTime);
              concatenateString(output, ", ");

              concatenateString(output, " OS: Interrupted by process ");
              sprintf(intBuffer, "%d", tempPtr->processID);
 
              concatenateString(output, intBuffer);
              concatenateString(output, " ");
              concatenateString(output, tempPtr->command);
              concatenateString(output, "end");
              *outputHead = addToOutputList(*outputHead, output, configPtr);

              tempPtr->reported = True;

              return True;

               
            }

         }
         tempPtr = tempPtr->nextNode;
      }
      return False;
   } 
   else
   {
      //check if io should be done and clear any finished io
      while(*interruptHead != NULL && tempPtr != NULL)
      {
         if(currentTime > tempPtr->endTime)
         {
            //get to process I/O came from
            ProcessControlBlock *processPtr = *queueHead;
            ProcessControlBlock **processToModify;
            while((processPtr)->processID != tempPtr->processID)
            {
               (processPtr) = (processPtr)->nextNode;
            }
            //increment next instruction pointer for process I/O came from
            processToModify = &processPtr;
            (*processToModify)->nextOpCommand = (*processToModify)->nextOpCommand->nextNode;
            //change process from waiting to ready
            (*processToModify)->processState = READY;

           //if interrupt not already reported, report
           if(tempPtr->reported == False)
           {
              currentTime = accessTimer(LAP_TIMER, timeBuffer);
              sprintf(output, "%f", currentTime);
              concatenateString(output, ", ");

              concatenateString(output, " OS: Process ");
              sprintf(intBuffer, "%d", (*processToModify)->processID);
 
              concatenateString(output, intBuffer);
              concatenateString(output, " ");
              concatenateString(output, tempPtr->command);
              concatenateString(output, "end");
              *outputHead = addToOutputList(*outputHead, output, configPtr);
               
           }
       
            //remove interrupt from queue
            *interruptHead = dequeue(*interruptHead, tempPtr);

         }
         tempPtr = tempPtr->nextNode;
      }

   }
   return False;
}

/*
Function Name: processLine
Algorithm: reports time remaining, command, and starting/ending
Precondition: given pointer to process control block, config pointer,
              and head of linked list containing output, head of process queue.
              and head of interrupt queue   
Postcondition: adds report of start and end to linked list containing output
Exceptions: none
Notes: none
*/
outputNode *processLine( outputNode *headPtr, ConfigDataType *configPtr,
                     ProcessControlBlock **currentProcess, ProcessControlBlock *processQueueHead,
                     ioData **interruptHead )
{
   //initialize variables
   char header[STD_STR_LEN];
   char processCmd[STD_STR_LEN];
   char startOutput[MAX_STR_LEN];
   char endOutput[MAX_STR_LEN];
   char intBuffer[STD_STR_LEN];
   char timeBuffer[STD_STR_LEN];
   int commandTime;
   int *commandTimePtr;
   int cpuCyclesCompleted;
   double currentTime;
   pthread_t tid;
   Boolean preemptive;
   OpCodeType *currentCommand = (*currentProcess)->nextOpCommand; 

   if(configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE
           || configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE
           || configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE)
      {
            preemptive = True;
      }
     else
     {
        preemptive = False;
     } 

   

   //report time 
   currentTime = accessTimer(LAP_TIMER, timeBuffer);

   //report initial information at start
   sprintf(header, "%s", " Process: ");
   sprintf(intBuffer, "%d", currentCommand->pid);
   concatenateString(header, intBuffer);
   concatenateString(header, ", ");

   //report time
   sprintf(startOutput, "%f", currentTime);
   concatenateString(startOutput, ", ");


   
   //report start of command
   //check for system start
   if(compareString(currentCommand->command, "sys") == STR_EQ)
   {

      //print OS
      concatenateString(startOutput, " OS: Simulator start");
      
      headPtr = addToOutputList(headPtr, startOutput, configPtr);
     
   }

   //check for app
   if(compareString(currentCommand->command, "app") == STR_EQ)
   {

      if(compareString(currentCommand->strArg1, "end") == STR_EQ)
      {
       
        //modify process state 
         (*currentProcess)->processState = EXITING;
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(startOutput, "%f", currentTime);
         concatenateString(startOutput, ", ");

         concatenateString(startOutput, " OS: Process ");
         sprintf(intBuffer, "%d", (*currentProcess)->processID);
 
         concatenateString(startOutput, intBuffer);
         concatenateString(startOutput, " ");
         concatenateString(startOutput, "set from RUNNING to EXITING");
 
         headPtr = addToOutputList(headPtr, startOutput, configPtr);
      }

   }

   //check for cpu
   if(compareString(currentCommand->command, "cpu") == STR_EQ)
   {
      //print process
      concatenateString(startOutput, header);

     
      //print second command
      
      sprintf(processCmd, "%s", currentCommand->command);
      concatenateString(processCmd, " ");

      //print str arg
      concatenateString(processCmd, currentCommand->strArg1);
      
      //report start
      concatenateString(startOutput, processCmd);
      concatenateString(startOutput, " start");
      headPtr = addToOutputList(headPtr, startOutput, configPtr);

      //get command time and number of cycles
      commandTime = configPtr->procCycleRate;
      commandTimePtr = &commandTime;
      cpuCyclesCompleted = 0;
      while((*currentProcess)->cpuCyclesLeft > 0 && (*currentProcess)->processState == RUNNING)
      {
         //run timer
         pthread_create(&tid, NULL, runTimer, (void *)commandTimePtr);
         pthread_join(tid, NULL);
      
         //increment completed cycles
         cpuCyclesCompleted++;

         //update time remaining, subtract one cycle time
         (*currentProcess)->timeRemaining -= configPtr->procCycleRate;
         //reduce number of cycles remaining
         (*currentProcess)->cpuCyclesLeft--;

         //check for interrupts
         if(preemptive == True && manageInterrupts(CHECK_FOR_INTERRUPTS, NULL, interruptHead, 
                   &processQueueHead, &headPtr, configPtr) == True)
         {
             (*currentProcess)->processState = READY;
             //if we finished cycles, move command forward
             if((*currentProcess)->cpuCyclesLeft == 0)
             {
                (*currentProcess)->nextOpCommand = 
                   (*currentProcess)->nextOpCommand->nextNode;  
             }
         }
         //if preemptive, check for time quantum
         if(preemptive == True)
         {
            if(cpuCyclesCompleted == configPtr->quantumCycles)
            {
                currentTime = accessTimer(LAP_TIMER, timeBuffer);
                sprintf(endOutput, "%f", currentTime);
                concatenateString(endOutput, ", ");
                concatenateString(endOutput, header);
                concatenateString(endOutput, "quantum time out, ");
                concatenateString(endOutput, processCmd);
                concatenateString(endOutput, " operation end");
                headPtr = addToOutputList(headPtr, endOutput, configPtr);
                 
                //if we didn't finish cycles, set to ready 
                if((*currentProcess)->cpuCyclesLeft != 0)
                {
                   (*currentProcess)->processState = READY;
                }
            }
         }

      }
      //if this CPU operation was completed, update CPU cycles left 
      // for next CPU command
      if((*currentProcess)->cpuCyclesLeft == 0)
      { 
         (*currentProcess)->cpuCyclesLeft = getNextCpuCycles(*currentProcess);
      }
      if((*currentProcess)->processState == RUNNING)
      {
         //report end of command
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ", ");
         concatenateString(endOutput, header);
         concatenateString(endOutput, processCmd);
         concatenateString(endOutput, " end");
         headPtr = addToOutputList(headPtr, endOutput, configPtr);

      }

      
   }
   //check for i/o
   if(compareString(currentCommand->command, "dev") == STR_EQ)
   {
      ioData *ioCommand = (ioData *)malloc(sizeof(ioData));
      //print process
      concatenateString(startOutput, header);

      //print device
      sprintf(processCmd, "%s", currentCommand->strArg1);

      //check for input and report
      if(compareString(currentCommand->inOutArg, "in") == STR_EQ)
      {
         concatenateString(processCmd, " input operation ");
      }
      else
      {
         concatenateString(processCmd, " output operation ");
      }
     
      //set ioData command info to device and input/output info 
      if(preemptive == True)
      {
         copyString(ioCommand->command, processCmd);
         concatenateString(startOutput, "blocked for ");
         concatenateString(startOutput, processCmd);

         headPtr = addToOutputList(headPtr, startOutput, configPtr);

         //get process ID
         ioCommand->processID = (*currentProcess)->processID;


         //get command time
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         ioCommand->endTime = currentTime + (double)getCommandTime(currentCommand, configPtr) / 1000;
         ioCommand->nextNode = NULL;
 
         ioCommand->reported = False;
            
         //set process to waiting
         (*currentProcess)->processState = WAITING;

         //update time remaining
         (*currentProcess)->timeRemaining -= getCommandTime(currentCommand, configPtr);
         manageInterrupts(SIGNAL_INTERRUPT, ioCommand, interruptHead, 
                          &processQueueHead, &headPtr, configPtr);
      }
      else
      {
         concatenateString(startOutput, processCmd);
         concatenateString(startOutput, " start");

         headPtr = addToOutputList(headPtr, startOutput, configPtr);

          //get command time
         commandTime = getCommandTime(currentCommand, configPtr);
         commandTimePtr = &commandTime;

         //run timer
         pthread_create(&tid, NULL, runTimer, (void *)commandTimePtr);
         pthread_join(tid, NULL);

  
         //report end of command
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ", ");
         concatenateString(endOutput, header);
         concatenateString(endOutput, processCmd);
         concatenateString(endOutput, "end");
         headPtr = addToOutputList(headPtr, endOutput, configPtr);


      }

      
      
    
   }

   //check for mem
   if(compareString(currentCommand->command, "mem") == STR_EQ)
   {

      //initialize success/failure variable
      Boolean memOpStatus;

      //get time and create header
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(startOutput, "%f", currentTime);
         concatenateString(startOutput, ", ");
         concatenateString(startOutput, header);
         concatenateString(startOutput, "attempting mem ");
         concatenateString(startOutput, currentCommand->strArg1);
         concatenateString(startOutput, " request");
         headPtr = addToOutputList(headPtr, startOutput, configPtr);

      //check for mem allocate
      if(compareString(currentCommand->strArg1, "allocate") == STR_EQ)
      {

         //try to allocate memory and report attempt
         memOpStatus = allocateMemory(currentProcess, processQueueHead, currentCommand,
                                         configPtr);

         //get time
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ", ");
         concatenateString(endOutput, header);

         

           //if success, report mem allocated
           if(memOpStatus == True) 
           {
              concatenateString(endOutput, "successful mem allocate request");
           }

           //if failure, report failure
           else
           {
              concatenateString(endOutput, "failed mem allocate request");
           }

          headPtr = addToOutputList(headPtr, endOutput, configPtr);

       }
      else
      {
         //try to access memory and report attempt
         memOpStatus = accessMemory(*currentProcess, currentCommand);

         //get time
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ", ");
         concatenateString(endOutput, header);

         

           //if success, report mem allocated
           if(memOpStatus == True) 
           {
              concatenateString(endOutput, "successful mem access request");
           }

           //if failure, report failure
           else
           {
              concatenateString(endOutput, "failed mem access request");
           }
         headPtr = addToOutputList(headPtr, endOutput, configPtr);

      }   
      if(memOpStatus == False)
      {
         //if operation failed, report segmentation fault
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ",  OS: Process ");
         sprintf(intBuffer, "%d", (*currentProcess)->processID);
         concatenateString(endOutput, intBuffer);
         concatenateString(endOutput, " experiences segmentation fault");         
         headPtr = addToOutputList(headPtr, endOutput, configPtr);
        
         //set process to exiting and report
         currentTime = accessTimer(LAP_TIMER, timeBuffer);
         sprintf(endOutput, "%f", currentTime);
         concatenateString(endOutput, ", ");

         concatenateString(endOutput, " OS: Process ");
         sprintf(intBuffer, "%d", (*currentProcess)->processID);
 
         concatenateString(endOutput, intBuffer);
         concatenateString(endOutput, " ");
         concatenateString(endOutput, "set from RUNNING to EXITING");
         (*currentProcess)->processState = EXITING;


  
         headPtr = addToOutputList(headPtr, endOutput, configPtr);

         
      }
   }


   return headPtr;

  
}



