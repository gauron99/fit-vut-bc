/** * * * * * * * * * * * * * *
 *                            *
 * 		@brief IFJ 2020  	  *
 *       Custom string        *
 *                            *
 *  @author Sabina Gulcikova  *
 *    	  @see  str.c         *
 *                            *
 *      @date 10/11/2020      *
 *                            *
 * * * * * * * * * * * * * *  */

#include <stdlib.h>
#include <stdio.h>

#include "custom_string.h"
#include "error.h"

int initDynamicString(dynamicString *dynStr) {
	dynStr->str = ((char *) malloc(SIZE_ALLOC));

    if (dynStr->str == NULL) {
			fprintf(stderr, "INTERNAL ERROR: malloc failed");
		    return -1;
	}

    eraseDynamicString(dynStr);
	dynStr->alloc_size = SIZE_ALLOC;

	return SUCCESS;
}

int appendChar(dynamicString *s, char c) {
    if (s->length + 1 >= s->alloc_size) { 

	unsigned long new_size = s->length + SIZE_ALLOC;
		s->str = ((char *) realloc(s->str, new_size));

		if(s->str == NULL) {
			fprintf(stderr, "INTERNAL ERROR: realloc failed");
			return -1;
		}
		
		s->alloc_size = new_size;
    }

	   s->str[s->length] = c;

	   s->length++;
	   s->str[s->length] = '\0';

	   return SUCCESS;
}

char* convertToString(dynamicString *dynStr) {
  	char *converted;
	converted = (char *) malloc(sizeof(char) * dynStr->length);

	// if(result == NULL) {
	// 	printf("INTERNAL ERROR: malloc failed");
	// 	return error;
	// }

	for(unsigned int i = 0; i < dynStr->length; i++) {
		    converted[i] = dynStr->str[i]; 
	}
	return converted;
} 

void eraseDynamicString(dynamicString *dynStr) {
    dynStr->length = 0;
	dynStr->str[0] = '\0';
}

void freeDynamicString(dynamicString *dynStr) {
	free(dynStr->str);
}