/** * * * * * * * * * * * * * *
 *                            *
 * 		@brief IFJ 2020  	  *
 *       Custom string        *
 *        Header file 		  *	
 * 				              *
 *  @author Sabina Gulcikova  *
 *  @see Public course pages  *
 * 	 with private materials   *
 *         for str.h          *
 *      @date 10/11/2020      *
 *                            *
 * * * * * * * * * * * * * *  */


#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H
#define SIZE_ALLOC 8 /*size of one char*/

/** 
 * Structure representing dynamic string
 * str - current content of the string
 * length - current length of the string
 * alloc_size - size of allocated memory
 */
typedef struct {
	char *str;
	unsigned int length;
	unsigned int alloc_size;
} dynamicString;

/** 
 * Function initDynamicString initializes dynStr
 * in order to prepare it for further use. 
 * The memory is allocated to size of one character
 */
int initDynamicString(dynamicString *dynStr);

/** 
 * Function appendChar charcter c to the end of 
 * string s. The size of string is properly 
 * enlarged by the size of one character. 
 */
int appendChar(dynamicString *s, char c);

/** 
 * Function convertToString converts dynamicString
 * to a string literal of particular length.
 * Each character is copied from one string 
 * to another.
 */
char* convertToString(dynamicString *dynStr); 

/** 
 * Function eraseDynamicString erases the content
 * of dynStr character by character. This function
 * does not free up the memory.
 */
void eraseDynamicString(dynamicString *dynStr);

/**
 * This function frees all memory that was previously
 * allocated for usage of dynamicString dynStr.
 */
void freeDynamicString(dynamicString *dynStr);

#endif