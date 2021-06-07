#include "datatypes.h"
#include "configops.h"
#include "StringUtils.h"
/*
Function Name: addToOutputList
Algorithm: adds to end of linked list
Preconditon: given pointer to front of queue or null
Postcondition: creates new linked list or appends node
               to end of linked list
Exceptions: none
Notes: none
*/ 
outputNode *addToOutputList( outputNode *localPtr, char *newLine, ConfigDataType *configPtr )
{
   //check for local ptr assigned to null
   if(localPtr == NULL)
   {
      //allocate memory for new node
      localPtr = (outputNode *) malloc(sizeof(outputNode));
      //assign all values to newly created node
      copyString(localPtr->currentLine, newLine);
      if(configPtr->logToCode==LOGTO_MONITOR_CODE || configPtr->logToCode==LOGTO_BOTH_CODE)
      {
         printf("%s \n", newLine);
      }

      //assign next node to null
      localPtr->nextNode = NULL;

      return localPtr;

   }

  //assume not at end of list
  localPtr->nextNode = addToOutputList(localPtr->nextNode, newLine, configPtr);
  return localPtr;
   
}


/*
Function Name: clearOutputList
Algorithm: recursively iterates through output linked list,
           returns memory to OS from the bottom of the list upward
Precondition: linked list, with or without data
Postcondition: all node memory, if any, is returned to OS,
               return pointer (head) is set to nulll
Exceptions: none
Notes: none
*/
outputNode *clearOutputList( outputNode *localPtr )
   { 
      //check for local pointer not set to null(list not empty)
      if( localPtr != NULL )
      {      
         //call recursive function with next pointer
          clearOutputList( localPtr->nextNode );      
         //after recursive call, release memory to OS
            //function: free
         free( localPtr );   
         //set local pointer to null
         localPtr = NULL;
      }   
      //return null to calling function
      return NULL;
   }

void writeToFile(ConfigDataType *configPtr, outputNode *outputHead)
{
   FILE * filePtr = fopen(configPtr->logToFileName, "w");
   outputNode *localPtr = outputHead;
   while(localPtr != NULL)
   {
      fprintf(filePtr, localPtr->currentLine);
      fprintf(filePtr, "\n");
      localPtr = localPtr->nextNode;
   }

   fclose(filePtr);
}
  
