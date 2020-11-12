#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

#define ADD_ALLOC_SIZE 8

typedef struct
{
	char *str;
	unsigned int length;
	unsigned int alloc_size;
} dynamicString;

/** 
 *
 */
int initDynamicString(dynamicString *s);

/** 
 * 
 */
int appendChar(dynamicString *s, char c);

/** 
 * 
 */
void eraseDynamicString(dynamicString *s);

/**
 *
 */
void freeDynamicString(dynamicString *s);

/** 
 * 
 */
char* convertToString(dynamicString *s);

#endif