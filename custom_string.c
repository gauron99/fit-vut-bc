/** * * * * * * * * * * * * * *
 *                            *
 * 		@brief IFJ 2020  	  *
 *       Custom string        *
 * 				              *
 *  @author Sabina Gulcikova  *
 *  @see Public course pages  *
 * 	 with private materials   *
 *         for str.c          *
 *      @date 17/10/20        *
 *                            *
 * * * * * * * * * * * * * *  */

#include <stdlib.h>
#include <stdio.h>

#include "custom_string.h"
#include "error.h"

int initDynamicString(dynamicString *dynStr) {
	dynStr->str = ((char *) malloc(SIZE_ALLOC));

    if (dynStr->str == NULL) { /*checks whether allocation was successful*/
			fprintf(stderr, "INTERNAL ERROR: malloc failed");
		    return -1;
	}

    eraseDynamicString(dynStr); /*cleans up any possible mess in created string*/
	dynStr->alloc_size = SIZE_ALLOC;

	return SUCCESS;
}

int appendChar(dynamicString *s, char c) {
    if (s->length + 1 >= s->alloc_size) { /*if necessary, the size of string is enlarged by realloc*/

	unsigned long new_size = s->length + SIZE_ALLOC;
		s->str = ((char *) realloc(s->str, new_size));

		if(s->str == NULL) { /*checks whether allocation was successful*/
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

	if(converted == NULL) { /*checks whether allocation was successful*/
		fprintf(stderr, "INTERNAL ERROR: malloc failed");
		return converted;
	}

	for(unsigned int i = 0; i < dynStr->length; i++) { /*each character is copied individually*/
		    converted[i] = dynStr->str[i]; 
	}
	return converted;
} 

void eraseDynamicString(dynamicString *dynStr) { /*erases the content of string*/
    dynStr->length = 0;
	dynStr->str[0] = '\0';
}

void freeDynamicString(dynamicString *dynStr) { /*frees up allocated memory*/
	free(dynStr->str);
}
