//Pre-compiler directive
#ifndef METADATAOPS_H
#define METADATAOPS_H

//header files
#include "datatypes.h"
#include "StringUtils.h"
#include <stdlib.h>
#include <stdio.h>

//constants
//op code messages, starts at index 3 to allow for string error messages
typedef enum { MD_FILE_ACCESS_ERR = 3,
               MD_CORRUPT_DESCRIPTOR_ERR,
               OPCMD_ACCESS_ERR,
               CORRUPT_OPCMD_ERR,
               CORRUPT_OPCMD_ARG_ERR,
               UNBALANCED_START_END_ERR,
               COMPLETE_OPCMD_FOUND_MSG,
               LAST_OPCMD_FOUND_MSG } OpCodeMessages;

extern const int BAD_ARG_VAL;

//function headers
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode );
OpCodeType *clearMetaDataList( OpCodeType *localPtr );
void displayMetaData( OpCodeType *localPtr );
int getCommand( char *cmd, char *inputStr, int index );
int getCommandTime( OpCodeType *opCommand, ConfigDataType *configDataPtr );
Boolean getMetaData( char *fileName,
                      OpCodeType **opCodeDataHead, char *endStateMsg);
int getOpCommand( FILE *filePtr, OpCodeType *inData);
int getNumberArg( int *number, char *inputStr, int index );
int getStringArg( char *strArg, char *inputStr, int index );
Boolean isDigit( char testChar );
int updateEndCount( int count, char *opString );
int updateStartCount( int count, char *opString );
Boolean verifyFirstStringArg( char *strArg );
Boolean verifyValidCommand( char *testCmd );

#endif //METADATAOPS_H
