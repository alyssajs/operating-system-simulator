//Pre-compiler directive
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>

//GLOBAL CONSTANTS - may be used in other files
typedef enum { False, True } Boolean;

//config data structure
typedef struct ConfigDataType
   {
      double version;
      char metaDataFileName[ 100 ];
      int cpuSchedCode;
      int quantumCycles;
      int memAvailable;
      int procCycleRate;
      int ioCycleRate;
      int logToCode;
      char logToFileName[ 100 ];
   } ConfigDataType;

typedef struct ioData
   {
      char command [ 100 ];    //I/O command, used for output
      double endTime;          //time the operation should end
      int processID;           //process that requested this operation
      Boolean reported;        //whether or not this interrupt has been reported
                               // as complete - to prevent duplicate reporting
      struct ioData *nextNode; //used for interrupt queue
   } ioData;

typedef struct memBlock
   {
      int lowerBound; //lower bound of allocated memory block
      int upperBound; //upper bound of allocated memory block
      struct memBlock *nextAlloc; //pointer to next block as needed
   } memBlock;

typedef struct OpCodeType
   {
      int pid;                 //pid, added when PCB is created
      char command[ 5 ];       //three letter command quantity
      char inOutArg[ 5 ];      //for device in/out
      char strArg1[ 15 ];      //arg 1 descriptor, up to 12 chars
      int intArg2;             //cycles or memory, assumes 4 byte int
      int intArg3;             //memory, assumes 4 byte int
                               //also non/preemption indicator
      double opEndTime;        //size of time string returned from accessTimer
      struct OpCodeType *nextNode; //pointer to next node as needed      
   } OpCodeType;

typedef struct ProcessControlBlock
   {
      int processID;            //numerical ID of process
      int processState;         //int representing state - new, ready, 
                                  //running, or exiting
      int timeRemaining;        //time remaining for process to finish executing
      int cpuCyclesLeft;        //cycles left for current CPU instruction  
      OpCodeType *nextOpCommand;   //pointer to next instruction
      memBlock *allocdMem;      //pointer to first allocated memory block
      struct ProcessControlBlock *nextNode; //pointer to next node as needed
   } ProcessControlBlock;

//Linked list that stores lines to output to file or monitor
typedef struct outputNode
   {
      char currentLine[ 200 ];    //line of text to output
      struct outputNode *nextNode; //next line of text    
   } outputNode;
#endif //DATATYPES_H
