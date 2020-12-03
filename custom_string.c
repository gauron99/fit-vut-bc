#include <stdlib.h>
#include <stdio.h>

#include "custom_string.h"

int initDynamicString(dynamicString *s) {
    if (!(s->str = (char *) malloc(ADD_ALLOC_SIZE))) {
		    return -1;
	  }

    eraseDynamicString(s);
	  s->alloc_size = ADD_ALLOC_SIZE;

	  return 0;
}

int appendChar(dynamicString *s, char c) {
     if (s->length + 1 >= s->alloc_size) {
		     unsigned long new_size = s->length + ADD_ALLOC_SIZE;
		     if (!(s->str = (char *) realloc(s->str, new_size))) {
			        return -1;
		     }
		     s->alloc_size = new_size;
     }

	   s->str[s->length++] = c;
	   s->str[s->length] = '\0';

	   return 0;
}

void eraseDynamicString(dynamicString *s) {
    s->length = 0;
	s->str[s->length] = '\0';
}

void freeDynamicString(dynamicString *s) {
	  free(s->str);
}

char* convertToString(dynamicString *s) {
  	char *result = (char *) malloc(sizeof(char) * s->length);

	  for (unsigned int i = 0; i < s->length; i++) {
		    result[i] = s->str[i];
	  }

	  return result;
}