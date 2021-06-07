// header files
#include "StringUtils.h"

// constant declaration
const char NULL_CHAR = '\0';
const char SPACE = ' ';
const char COLON = ':';
const char COMMA = ',';
const char SEMICOLON = ';';
const char PERIOD = '.';
const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';
const int SMALL_STR_LEN = 30;
const int STD_STR_LEN = 60;
const int MAX_STR_LEN = 200;
const int STR_EQ = 0;
const int SUBSTRING_NOT_FOUND = -1001;
const Boolean IGNORE_LEADING_WS = True;
const Boolean ACCEPT_LEADING_WS = False;

/*
Function Name: getStringLength
Algorithm: linear count across string to find length
Precondition: C-Style string, having a null char ('\0')
              at end of string
Postcondition: length of string or number of characters
               up to null char, len >= 0
Exceptions: none
Notes: none
*/
int getStringLength( char *str )
  {
   // initialize function/variables
   int index = 0;

   // loop until null character found
   while( str[ index ] != NULL_CHAR )
      {
       // increment index
       index++;
      }

   // return index/count
   return index;
  }

/*
Function Name: copyString
Algorithm: copies contents of one string into another
Precondition: C-Style source string, having a null char ('\0')
              at end of string; destination string is passed
              with enough memory to accept source string
Postcondition: destination string contains an exact copy
               of the source string
Exceptions: limit on string loop in case incorrect string format
Notes: none
*/
void copyString( char *destination, char *source )
  {
   // initialize function/variables
   int index = 0;

   // loop until null character is found in source string
   //   loop limited to MAX_STR_LEN
   while( index < MAX_STR_LEN && source[ index ] != NULL_CHAR )
      {
       // assign source character to destination element
       destination[ index ] = source[ index ];

       // increment index
       index++;

       // assign null character to next destination element
       destination[ index ] = NULL_CHAR;
      }
  }

/*
Function Name: concatenateString
Algorithm: concatenates contents of one string onto another
Precondition: C-Style source string, having a null char ('\0')
              at end of string; destination string is passed
              with enough memory to accept additional concatenated string
Postcondition: destination string contains its original data 
               plus the concatenated source string
Exceptions: limit on string loop in case incorrect string format
Notes: none
*/
void concatenateString( char *destination, char *source )
  {
   // initialize function/variables

      // set destination index to length of destination string
         // function: getStringLength  
      int destIndex = getStringLength( destination );

      // set source index to zero
      int sourceIndex = 0;

   // loop to end of source index
   //   loop limited to MAX_STR_LEN
   while( destIndex < MAX_STR_LEN && source[ sourceIndex ] != NULL_CHAR )
      {
       // assign source character to destination at destination index
       destination[ destIndex ] = source[ sourceIndex ];

       // increment source and destination indices
       sourceIndex++; destIndex++;

       // assign null character to next destination element
       destination[ destIndex ] = NULL_CHAR;
      }
  }

/*
Function Name: compareString
Algorithm: compares two strings alphabetically such that:
           if oneStr < otherStr, the function will return a value < 0
           if oneStr == otherStr, the function will return 0
           if oneStr > otherStr, the function will return a value > 0
           if two strings are identical up to the point that one is longer,
           the difference in lengths will be returned
Precondition: two C-Style source strings, having a null char ('\0')
              at end of each string
Postcondition: integer value returned as specified
Exceptions: limit on string loop in case incorrect string format
Notes: none
*/
int compareString( char *oneStr, char *otherStr )
  {
   // initialize function/variables
   int difference, index = 0;

   // loop to end of one of the two strings
   //   loop limited to MAX_STR_LEN
   while( index < MAX_STR_LEN 
          && oneStr[ index ] != NULL_CHAR 
             && otherStr[ index ] != NULL_CHAR )
      {
       // find the difference between the currently aligned characters
       difference = oneStr[ index ] - otherStr[ index ];

       // check for non-zero difference
       if( difference != 0 )
          {
           // return non-zero difference
           return difference;
          }

       // increment index
       index++;
      }

   // assume strings are equal to this point, return string length differences
      // function: getStringLength
   return getStringLength( oneStr ) - getStringLength( otherStr );
  }

/*
Function Name: getSubString
Algorithm: captures sub string within larger string 
           between two inclusive indices
Precondition: C-Style source string, having a null char ('\0')
              at end of the string
Postcondition: substring returned as parameter
Exceptions: limit on string loop in case incorrect string format,
            empty string returned if either index is out of range
Notes: copy of source string is made internally to protect from aliasing
*/
void getSubString( char *destStr, char *sourceStr, 
                                                int startIndex, int endIndex )
   {
    // initialize function/variables

       // set length of source string
       int sourceStrLen = getStringLength( sourceStr );

       // initialize destination index to zero
       int destIndex = 0;

       // initialize source index to start index parameter
       int sourceIndex = startIndex;

       // create pointer to temp string
       char *tempSourceStr;

    // check for indices within limits
    if( startIndex >= 0 && endIndex >= startIndex && endIndex < sourceStrLen )
       {
        // create temporary string, copy source string to it
           // function: malloc, copyString
        tempSourceStr = (char *) malloc( sourceStrLen + 1 );
        copyString( tempSourceStr, sourceStr );

        // loop across requested substring (indices)
        //   loop limited to MAX_STR_LEN
        while( destIndex < MAX_STR_LEN && sourceIndex <= endIndex )
           {
            // assign source character to destination element
            destStr[ destIndex ] = tempSourceStr[ sourceIndex ];

            // increment indices
            destIndex++; sourceIndex++;
    
            // add null character to next destination string element
            destStr[ destIndex ] = NULL_CHAR;
           }

        // release memory used for temp source string
           // function: free
        free( tempSourceStr );
       }

    // otherwise, assume indices outside limits
    else
       {
        // create empty string with null character
        destStr[ 0 ] = NULL_CHAR;
       }
   }

/*
Function Name: findSubString
Algorithm: linear search for given substring within testString
Precondition: C-Style source testString, having a null char ('\0')
              at end of the string
Postcondition: index of substring location returned,
               or SUBSTRING_NOT_FOUND constant is returned
Exceptions: none
Notes: none
*/
int findSubString( char *testStr, char *searchSubStr )
  {
   // initialize function/variables

      // initialize test string length
         // function: getStringLength
      int testStrLen = getStringLength( testStr );

      // initialize other variables
      int searchIndex, internalIndex, masterIndex = 0;

   // loop across test string
   while( masterIndex < testStrLen )
      {
       // set internal loop index to current test string index
       internalIndex = masterIndex;

       // set internal search index to zero
       searchIndex = 0;

       // loop to end of test string 
       //   while test string/substring characters are the same
       while( internalIndex <= testStrLen 
              && testStr[ internalIndex ] == searchSubStr[ searchIndex ] )
          {
           // increment test string, substring indices
           internalIndex++; searchIndex++;

           // check for end of substring (search completed)
           if( searchSubStr[ searchIndex ] == NULL_CHAR )
              {
               // return beginning test string index
               return masterIndex;
              }
          }

       // increment current test string index
       masterIndex++;
      }

   // return substring not found
   return SUBSTRING_NOT_FOUND;
  }

/*
Function Name: setStrToLowerCase
Algorithm: iterates through string, sets all upper case letters
           to lower case
Precondition: C-Style source sourceString, having a null char ('\0')
              at end of the string; destination string has memory
              to hold source string with modified letters
Postcondition: all upper case letters in given string are set
               to lower case; no change to any other characters
Exceptions: limit on string loop in case incorrect string format
Notes: copy of source string is made internally to protect from aliasing;
       uses setCharToLowerCase
*/
void setStrToLowerCase( char *destStr, char *sourceStr )
   {
    // initialize function/variables

       // create temporary source string
          // function: getStringLength, malloc
       int strLen = getStringLength( sourceStr );
       char *tempStr = (char *) malloc( strLen + 1 );

       // initialize source string index to zero
       int index = 0;

       // copy source string to temp
          // function: copyString
       copyString( tempStr, sourceStr ); 

    // loop to end of temp/source string
    //   loop limited to MAX_STR_LEN
    while( index < MAX_STR_LEN && tempStr[ index ] != NULL_CHAR )
       {
        // change letter to lower case as needed and assign
        //   to destination string
           // function: setCharToLowerCase
        destStr[ index ] = setCharToLowerCase( tempStr[ index ] );

        // increment index
        index++;

        // add null character to next destination string element
        destStr[ index ] = NULL_CHAR;      
       }

    // release temp string memory to OS
       // function: free
    free( tempStr );
   }

/*
Function Name: setCharToLowerCase
Algorithm: sets upper case letter to lower case;
           has no effect on any other characters
Precondition: given character value
Postcondition: upper case letter is set to lower case;
               no change to any other characters
Exceptions: none
Notes: none
*/
char setCharToLowerCase( char inputChar )
   {
    // check for character between 'A' and 'Z'
    if( inputChar >= 'A' && inputChar <= 'Z' )
       {
        // convert upper case letter to lower case
        inputChar = inputChar - 'A' + 'a';
       }

    // return character
    return inputChar;
   }

/*
Function Name: getLineTo
Algorithm: finds given text in file, skipping white space if specified,
           stops searching at given character or length          
Precondition: file is open with valid file pointer;
              char buffer has adequate memory for data;
              stop character and length are valid
Postcondition: ignores leading white space if specified;
               captures all characters and stores in string buffer
               up to stop character or given length;
               non-printable characters are not stored in buffer
Exceptions: returns INCOMPLETE_FILE_ERR if no valid data found;
            returns NO_ERR if operation is success
Notes: none
*/
int getLineTo( FILE *filePtr, int bufferSize, char stopChar, 
       char *buffer, Boolean omitLeadingWhiteSpace, Boolean stopAtNonPrintable )
   {
   // initialize function/variables
    int charAsInt, charIndex = 0;
    int statusReturn = NO_ERR;
    Boolean bufferSizeAvailable = True;
    Boolean nonPrintableFound = False;
    
    // get first character (as integer)
       // function: fgetc
    charAsInt = fgetc( filePtr );

    // consume leading white space, if flagged
    while( omitLeadingWhiteSpace == True
           && charAsInt != (int) stopChar
           && charIndex < bufferSize 
           && charAsInt <= (int) SPACE )
       {
        // get next character (as integer)
           // function: fgetc
        charAsInt = fgetc( filePtr );
       }

    // check for stop at non printable if needed
    if( stopAtNonPrintable == True && charAsInt < (int)SPACE )
       {
        // set non printable flag to true
        nonPrintableFound = True;
       }

    // capture string
    // loop while character is not stop character and buffer size available
    while( charAsInt != (int) stopChar 
                 && nonPrintableFound == False && bufferSizeAvailable == True )
       {
        // check for input failure
           // function: isEndOfFile
        if( isEndOfFile( filePtr ) == True )
           {
            // return incomplete file error
            return INCOMPLETE_FILE_ERR;
           }

        // check for usable (printable) character
        if( charAsInt >= (int) SPACE )
           {
            // assign input character to buffer string
            buffer[ charIndex ] = (char) charAsInt;

            // increment index
            charIndex++;
           }

        // set next buffer element to null character
        buffer[ charIndex ] = NULL_CHAR;

        // check for not at end of buffer size
        if( charIndex < bufferSize - 1 )
           {
            // get a new character
            charAsInt = fgetc( filePtr );

            // check for stop at non printable if needed
            if( stopAtNonPrintable == True && charAsInt < (int)SPACE )
               {
                // set non printable flag to true
                nonPrintableFound = True;
               }
           }

        // otherwise, assume end of buffer size
        else
           {
            // set buffer size Boolean to false to end loop
            bufferSizeAvailable = False;

            // set status return to buffer overrun
            statusReturn = INPUT_BUFFER_OVERRUN_ERR;
           }
       }

    // return operation status
    return statusReturn;
   }

/*
Function Name: isEndOfFile
Algorithm: reports end of file, using feof         
Precondition: file is opened with valid file pointer
Postcondition: reports end of file
Exceptions: none
Notes: none
*/
Boolean isEndOfFile( FILE *filePtr )
   {
    // check for feof end of file response
    if( feof( filePtr ) != 0 )
       {
        // return true
        return True;
       }

    // return false
    return False;
   }


