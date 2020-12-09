/**
          ~~ IFJ-20 ~~
      @file   precanalysis.h
      @brief  header file for precanalysis.c
      @author Martin Zaťovič
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"

#ifndef PREC
    #define PREC

    #define INIT_ERR -42
    #define STACK_SIZE 20   // default stack size
    #define STACK_TOP -1    // default index of the top of the stack
    
    #define NOT_FOUND -658  // err const, used when rule wasnt found
    #define SYMBOL_NOT_RECOGNIZED -659
    #define DIFFERENT_TYPES -660    // err const, used when types dont match
    #define NO_TYPE_OPERATION -661
    #define MALLOC_ERR -662

    #define PA_ERR -1
    #define SYNTAX_ERR -2
    #define SEM_ERR -3
    #define SEM_ERR_COMP_MINUS -5
    #define ID_SIZE 100

    // global array that contains rules used by PA automata
    extern int rules[25][25];

    /**
     * @brief symbol structutre
     * paToken pointer points to corresponding Token
     * paType parameter type used by PA
     */
    typedef struct sElemType{
        Token *paToken;
        int paType;
        char *name;
    } sElemType;
    
    /**
     * @brief symbol stack struct, consists of a pointer to the stack, current top index and size
     * stack pointer symbol stack array
     * top parameter index of the top of the symbol stack
     * size parameter size of the symbol stack
     */
    typedef struct s_t{
        sElemType **stack;
        int top;
        int size;
    } s_t; // pri refaktorizacii s_t premenovat nakoniec, moze sa vyskytovat aj inde

    /**
     * @brief integer stack structure
     * iStack pointer integer stack array
     * top parameter index of the top of the integer stack
     * size parameter size of the integer stack
     */
    typedef struct {
        int **iStack;
        int top;
        int size;
    } is_t;
 
    /**
     * @brief enumeration of precedence table values
     */
    typedef enum {
        PA_LESS = 420,
        PA_GREATER,
        PA_SHIFT,
        PA_EMPTY,
    } pTableValue;
   
    /**
     * @brief enumeration of types used by PA
     */
    typedef enum {
        OP_PLUS,        // 0
        OP_MINUS,       // 1
        OP_MUL,         // 2
        OP_DIV,         // 3

        OP_LBRAC,       // 4
        OP_RBRAC,       // 5
        
        OP_LESS,        // 6
        OP_LESS_EQUAL,  // 7
        OP_MORE,        // 8
        OP_MORE_EQUAL,  // 9
        OP_EQUAL,       // 10
        OP_NOT_EQUAL,   // 11

        OP_NOT,         // 12
        OP_AND,         // 13
        OP_OR,          // 14

        OP_COMMA,       // 15
        
        OP_FUN,         // 16

        OP_DOLLAR,      // 17
        
        OP_INTEGER,     // 18
        OP_FLOAT,       // 19
        OP_STRING,      // 20

        OP_ID,          // 21
        
        OP_EXPRESSION,  // 22

        OP_TABLE_LESS,  // 23
        OP_BOOL,        // 24
        OP_SEPARATOR,   // 25
    } paType;
   
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTIONS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    
    /**
     * @brief initializes integer stack
     * @param intStack integer stack to be initialized
     * @return bool value, true if initialization was successful, false if not
     */
    bool intStackInit(is_t *intStack);
    
    /**
     * @brief frees integer stack
     * @param intStack integer stack to be freed
     */
    void intStackFree(is_t *intStack);

    /**
     * @brief is integer stack full?
     * @param integer stack to be analyzed
     * @return bool value, true if stack is full, false if not
     */
    bool intStackIsFull(is_t *intStack);

    /**
     * @brief is integer stack empty?
     * @param intStack integer stack to be analyzed
     * @return bool value, true if stack is empty, false if not
     */
    bool intStackIsEmpty(is_t *intStack);

    /**
     * @brief pushes a number onto integer stack
     * @param intStack integer stack to which a number is to be pushed 
     * @return bool value, true if push succeeded, false if not
     */
    bool intStackPush(is_t *intStack, int data);
     
     /**
     * @brief pop the first symbol of a symbol stack and the symbol
     * @param symStack symbol stack from which a symbol is to be popped
     */
    int intStackPop(is_t *intStack);

    /**
     * @brief initializes precedence table
     * @param table empty table to be initialized
     * @return bool value, true if initialization was successful, false if not
     */
    bool pTableInit(int table[25][25]);   // v prectableandrules.c
    

    
    /**
     * @brief symbol stack initialization, s must be allocated beforehand
     * @param symStack stack to be initialized
     * @return bool value, true if init was successful, false if not
     */
    bool sInit(s_t* symStack);

    /**
     * @brief symbol stack destructor
     * @param symStack stack to be freed
     */
    void sFree(s_t *symStack);        

    /**
     * @brief is symbol stack empty?
     * @param symStack symbol stack to be analyzed
     * @return returns bool value
     */
    bool sIsEmpty(s_t* symStack);
    
    /**
     * @brief is symbol stack full?
     * @param symStack symbol stack to be analyzed
     * @return returns bool value
     */
    bool sIsFull(s_t* symStack);     

    /**
     * @brief copy the first symbol on the stack into a structure
     * @param symStack symbol stack to be analyzed
     * @param data structure to be filled with the first symbol
     */
    void sGetTop(s_t* symStack, sElemType *data);

    /**
     * @brief gets a pointer to the top of the symbol stack
     * @param symStack symbol stack, top of which top be returned
     * @param data structure pointer used as a return variable
     */
    void sGetTopPointer(s_t *symStack, sElemType *data); 
 
    /**
     * @brief pop the first term of a symbol stack and free each of its attributes
     * @param symStack symbol stack from which a symbol is to be popped
     */   
    void sPop(s_t* symStack);
    
    /**
     * @brief pop the first symbol of a symbol stack and the symbol
     * @param symStack symbol stack from which a symbol is to be popped
     */   
    void sPopPointer(s_t *symStack);     
    
    /**
     * @brief allocates memory for a new symbol on the stack and copies pushData into it, then makes it the first element of the symbol stack
     * @param symStack symbol stack to which a symbol is to be be pushed
     * @param pushData symbol to be pushed
     * @return bool value, true if push succeeded, false if not
     */
    bool sPush(s_t* symStack, sElemType *newData);
    
    /**
     * @brief function analogical to sPush, only works with a pointer
     * @param symStack symbol stack to which a symbol is to be be pushed
     * @param pushData symbol to be pushed
     * @return bool value, true if push succeeded, false if not
     */
    bool sPushPointer(s_t *symStack, sElemType *data);
    
    /**
     * @brief initialization of a symbol structure
     * @param data pointer to a symbol structure to be initialized
     */
    void sElemInit(sElemType *data);
    
    /**
     * @brief frees a symbol structure, requires pointer to the structure AND a pointer to paToken to be successful
     * @param data symbol structure to be freed
     * @param paToken paToken from the symbolStructure
     */
    void sElemFree(sElemType *data, Token *paToken);     
    
    /**
     * @brief fill a symbol structure with data from a Token
     * @param token token to be analyzed
     * @param data symbol structure to be filled
     * @return bool value, true if success, false if not
     */
    bool sElemGetData(Token *token, sElemType *data); 

    /**
     * @brief copies symbols on the top of a symbol stack until the first terminal symbol into another symbol stack
     * @param src source symbol stack
     * @param dst destination symbol stack 
     * @param helper a helper structure for a symbol
     * @return bool value, true if copying was successful, false if not
     */
    bool sCopyUntilTerminal(s_t *src, s_t *dest, sElemType *helper);

    /**
     * @brief copies all symbols from a symbol stack into another symbol stack
     * @param src source symbol stack
     * @param dst destination symbol stack 
     * @param helper a helper structure for a symbol     
     * @return bool value, true if copying was successful, false if not
     */
    bool sCopyAll(s_t *src, s_t *dest, sElemType *helper);    
    
    /**
     * @brief finds the first terminal symbol on a symbol stack
     * @param symStack symbol stack to be analyzed
     * @return paType of the first terminal on symbol stack
     */
    int sFindFirstTerminal(s_t *symStack); 
    
    /**
     * @brief determines paType of a token
     * @param token token to be analyzed
     * @return paType of a specified Token
     */
    int getPaType(Token *t);
    
    /**
     * @brief compares the rule on the stack with a predefined set of rules and stores important data like datatypes
     * @param rule rule on the helper stack
     * @param typeStack stack to store types of symbols, even types of Expressions
     * @param lastFoundType the type of last analyzed symbol/expression
     * @param Tekken Token of the first symbol of a rule, useful for IDENTIFIERS /to get its type/
     * @return paType of a specified Token
     */
    int generateRule(int *rule, is_t *typeStack, int *lastFoundType, Token *Tekken, char *name, char *value1, char *value2);

    /**
     * @brief tries to find a rule that shall be used
     * @param mainStack symbol stack to be analyzed
     * @param helperStack a helper symbol stack
     * @param helperTerminal helper symbol structure
     * @param typeStack int stack used for keeping track of types of expressions after being rewritten from datatypes and identifiers
     * @return int value representing index of a suitable rule to be used from the array of rules 
     */
    int  sFindRule(s_t *mainStack, s_t *helperStack, sElemType *helperTerminal, is_t *typeStack, int *lastFoundType, char *name);

    /**
     * @brief main PA function
     * @return returns code of negative error code if PA finds invalid expression, or the final type of the analyzed expression
     */
    int analyzePrecedence();

#endif
