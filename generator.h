/**
        ~~ IFJ-20 ~~
    @file   generator.h
    @brief  header file for generator.c  
    @date   09/12/20
    @author David Fridrich
*/


#ifndef GENERATOR_H
#define GENERATOR_H

#include "error.h"
/**
 * @brief structure for 3 adress code
 * @param name name of instruction
 * @param boku first argument
 * @param no second argument
 * @param pico third argument
 */
typedef struct trAK {
    char* name;

    char* boku;
    char* no;
    char* pico;
}trAK;
/**
 * @brief stores all DEFVARS passed to generator.c
 */
extern char **defVars;
/**
 * @brief stores how many values defVars holds
 */
extern long long int lenOfArr;


/** function declarations **/
/**
 * @brief help function for generation of labels/identifiers
 */
int count_digits(unsigned long long int n);
/**
 * @brief returns unique identifier (ex: $$$$1)
 */
char* generate_identifier();
/**
 * @brief returns unique label (ex: %%%%3)
 */
char* generate_label();

/**
 * @brief inicializes output for interpret (.IFJCODE20...) 
 */
void generateBeginningOfExistence();
/**
 * @brief generate built-in functions
 *
 * prints code in ifj20 language of built-in functions
 */
void generateInBuilt();
/**
 * @brief generates code in ifjcode20 based on argument
 * @param instr instr is a struct for 3AK instruction 
 */
int generate(trAK instr);
/**
 * @brief inicializes variables needed in generator.c
 */
void initInC();
/**
 * help function for concatenation of strings (for "LF@" etc.)
 */
char* concat(const char *s1, const char *s2);

#endif