/**
        ~~ IFJ-20 ~~
    @file   generator.h
    @brief  header file for generator.c  
    @date   11/11/20
    @author David Fridrich
*/


#ifndef GENERATOR_H
#define GENERATOR_H

#include "error.h"

typedef struct trAK {
    char* name;

    char* boku;
    char* no;
    char* pico;
}trAK;

extern char **defVars;
extern long long int lenOfArr;


/** function declarations **/
int count_digits(unsigned long long int n);
char* generate_identifier();
char* generate_label();

void generateBeginningOfExistence();
/**
 * @brief generate built-in functions
 *
 * prints code in ifj20 language of built-in functions
 */
void generateInBuilt();
/**
 * @brief main function of generator.c 
 */
int generate(trAK instr);

void initInC();

char* concat(const char *s1, const char *s2);

#endif