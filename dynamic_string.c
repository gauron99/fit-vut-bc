#include <stdlib.h>
#include <stdio.h>

#include "dynamic_string.h"

int initDstring(dstring *s) {
    if (!(s->str = (char *) malloc(ADD_ALLOC_SIZE))) {
		    return -1;
	  }

    dynamic_string_clear(s);
	
	s->alloc_size = ADD_ALLOC_SIZE;

	return 0;
}

int addChar(dstring *s, char c) {
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

void dynamic_string_clear(dstring *s) {
    s->length = 0;
	  	s->str[s->length] = '\0';
}

void dynamic_string_free(dstring *s) {
	  free(s->str);
}

char* dynamic_to_string(dstring *s) {
  	char *result = (char *) malloc(sizeof(char) * s->length);

	for (unsigned int i = 0; i < s->length; i++) {
		result[i] += s->str[i];
	}

	return result;
}
