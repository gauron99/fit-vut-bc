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

typedef enum instructionNames{
    FUNC_DEF, //label, createframe, pushframe
    FUNC_DEF_END, //popframe, return
    DEFVAR, //defvar <varName> <value>
    POPS,
    PUSHS,
    MOVE, //assignment --> a = 5 (just a MOVE instr)
    IF_DEF, //DEFVAR var of type bool- present how to get out of the IF (aka skip it)
    IF_END, // LABEL name of where to jump if IF is not to be executed
    FOR_DEF, //DEF LABEL to jump to, iterator, and when to get out
    FOR_END, // iterator +1, JUMP to LABEL back

}instrNames;
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