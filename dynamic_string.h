#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#define ADD_ALLOC_SIZE 8

typedef struct
{
	char *str;
	unsigned int length;
	unsigned int alloc_size;
} dstring;


int initDstring(dstring *s);

int addChar(dstring *s, char c);

void dynamic_string_clear(dstring *s);

void dynamic_string_free(dstring *s);

char* dynamic_to_string(dstring *s);

#endif