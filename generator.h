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

// typedef enum instructionNames{
//     FUNC_DEF, //label, createframe, pushframe //done
//     FUNC_DEF_END, //popframe, return //done
//     DEFVAR, //defvar //done
//     POPS, //done
//     PUSHS, //done
//     MOVE, //assignment --> a = 5 (just a MOVE instr) //done

//     IF_DEF, //<variable na bool> <dalsi label> //TODO
//     IF_END, // LABEL name of where to jump if IF is not to be executed //TODO
//     FOR_DEF, //DEF LABEL to jump to, iterator, and when to get out //TODO
//     FOR_END, // iterator +1, JUMP to LABEL back //TODO

//     ADD, //done
//     SUB, //done
//     DIV, //done
//     MUL, //done
//     CONCAT,//done
//     JUMP //done
//     PRINT //done
/*     //--------------------------- TODO ---------------------------
*/
// }instrNames;

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
 * function will be the base of the whole file
 * TODO -- arguments from parser( array of tokens or something)
 */
int generate(trAK instr);

void initInC();


#endif