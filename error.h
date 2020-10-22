/*
    @filename   error.h
*   
    @brief list of error codes
*    
    @author Sabina Gulcikova
*    
    @date 28/09/20
*
        IFJ-2020

*/ 

#ifndef ERROR_H 
#define ERROR_H


/* ------------------ NO ERRORS ------------------*/
/**
 *  error code: 0
 *  description: when everything goes how it should
 */

#define SUCCESS 0

/* ------------------ LEXICAL ERRORS -------------------*/

/**
 * error code: 1
 * description: error of the input file discovered by lexical analysis (invalid structure of the current lexeme)
 */ 

#define LEXICAL_ERROR 1

/* ------------------ SYNTACTIC ERRORS -------------------*/

/**
 * error code: 2
 * description: error of the input file discovered by syntactic analysis (invalid syntax, unexpected end of line)
 */

#define SYNTACTIC_ERROR 2


/* ------------------ SEMANTIC ERRORS -------------------*/

/**
 * error code: 3
 * description: semantic error of the input file caused by:
 *              undefined function / variable
 *              redefined function / variable, etc.
 */ 
#define SEM_ERROR_DEF 3

/**
 * error code: 4
 * description: semantic error caused by derivation of data types of newly defined variable
 * type: SEMANTIC ERROR
 */
#define SEM_ERROR_TYPE 4

/**
 * error code: 5
 * decription: semantic error caused by:
 *             incompatible types in arithmetic, string and other expressions
 */
#define SEM_ERROR_COMP 5

/**
 * error code: 6
 * description: semantic error in the input file 
 *  caused when calling / returning from a function by:
 *              invalid number / type of parameters 
 *              invalid number / type of return values
 */
#define SEM_ERROR_COUNT 6

/**
 * error code: 7
 * description: other semantic errors
 */
#define SEM_ERROR_OTHER 7

/**
 * error code: 9
 * description: semantic error of division by zero
 */
#define SEM_ERROR_DIV_ZERO 9

/* ------------------ INTERNAL ERRORS -------------------*/

/**
 * error code: 99
 * description: internal error of the compiler 
 *              error was not caused by the input file (e.g. memory allocation error)
 */
#define INTERNAL_ERROR 99

#endif // !ERROR_H
