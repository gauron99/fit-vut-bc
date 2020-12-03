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

/** function declarations **/


void generateBeginningOfExistence();
/**
 * @brief generate built-in functions
 *
 * prints code in ifj20 language of built-in functions
 */
void generateInBuilt();
/**
 * @brief main function of generator.c 
 * function will be the base of the whole file
 * TODO -- arguments from parser( array of tokens or something)
 */
int generate(trAK instr);


#endif