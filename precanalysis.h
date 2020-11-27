#ifndef PREC
#define PREC

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"

#define OP_CNT 17
#define INIT_ERR -42
#define STACK_SIZE 20
#define STACK_TOP -1
#define NOT_FOUND -658
#define DIFFERENT_TYPES -659

// co zacina s_ ma do cinenia so stackom
typedef struct {
    Token *paToken;
    int paType;
} sElemType;

typedef struct {
    sElemType **stack;
    int top;
    int size;
} s_t; // pri refaktorizacii s_t premenovat nakoniec, moze sa vyskytovat aj inde

enum {
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
    
    OP_INTEGER,
    OP_FLOAT,
    OP_STRING,

    OP_ID,          // 19
    
    OP_EXPRESSION,  // 20

    OP_TABLE_LESS,  // 21
};

// ***RULES***
extern int rules[25][25];


enum {
    PA_LESS = 420,
    PA_GREATER,
    PA_SHIFT,
    PA_EMPTY,
};

// initializes precedence table
int pTableInit(int table[25][25]);   // v prec_table_init.c

// determines paType of a token
int getPaType(Token *t, s_t *typeStack);

// S T A C K  F U N C T I O N S //

bool sInit(s_t* s);        
// stack initialization, s must be allocated beforehand
void sFree(s_t *s);        
// stack destructor

bool sIsEmpty(s_t* s);    
// is stack empty?
bool sIsFull(s_t* s);     
// is stack full?

void sGetTop(s_t* s, sElemType *data);     
// copy the stack top term to data
void sGetTopPointer(s_t *s, sElemType *data);     
// make data's pointers point to the attributes of the first term of the stack

void sPop(s_t* s);         
// pop the first term and free each of its attributes (paToken->(union)value)
void sPopPointer(s_t *s);     
// pop the first term and free the term itself only
bool sPush(s_t* s, sElemType *newData);     
// allocates memory for a new term on the stack and for each of its attributes,
// then copies newData into it and pushes it onto the stack
bool sPushPointer(s_t *s, sElemType *data);        
// reallocates stack memory if its too small, allocates a new stack data
// structure and makes each of its attributes point to attributes of data, then
// pushes it onto the stack

void sElemInit(sElemType *data);       
// sElemType structure initialization
void sElemFree(sElemType *data, Token *paToken);      
// frees sElemType structure, requires pointer to the structure AND pointer to 
// paToken to be successful

int  sFindFirstTerminal(s_t *s); 
// scans the stack to find a terminal
bool sCopyUntilTerminal(s_t *src, s_t *dest, sElemType *helper); 
// while there is EXPRESSION or < on the top of the stack, pushes it to dest and pops it (the pushed terms are saved in reversed order)
bool sCopyAll(s_t *src, s_t *dest, sElemType *helper); 
// copies the stack to dest in reversed order

int  sFindRule(s_t *mainStack, s_t *helperStack, sElemType *helperTerminal, s_t *typeStack);
// gets the elements on top of the stack until < and sends them to compareRules

int compareRules(int *rule);
// 

bool sElemGetData(Token *token, sElemType *data, s_t *typeStack);

Token * get_token(Token t_stream[15], int pos);

int analyzePrecedence();

#endif
