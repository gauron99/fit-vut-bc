#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H
#define SIZE_ALLOC 8

typedef struct {
	char *str;
	unsigned int length;
	unsigned int alloc_size;
} dynamicString;

/** 
 *
 */
int initDynamicString(dynamicString *dynStr);

/** 
 * 
 */
int appendChar(dynamicString *s, char c);

/** 
 * 
 */
char* convertToString(dynamicString *dynStr); 

/** 
 * 
 */
void eraseDynamicString(dynamicString *dynStr);

/**
 *
 */
void freeDynamicString(dynamicString *dynStr);

#endif