#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "scanner.h"

int pTableInit(int table[15][15]) {
    table[OP_PLUS][OP_PLUS] = PA_GREATER;
    table[OP_PLUS][OP_MINUS] = PA_GREATER;
    table[OP_PLUS][OP_MUL] = PA_LESS;    
    table[OP_PLUS][OP_DIV] = PA_LESS;
    table[OP_PLUS][OP_LBRAC] = PA_LESS;
    table[OP_PLUS][OP_RBRAC] = PA_GREATER;
    table[OP_PLUS][OP_LESS] = PA_GREATER;
    table[OP_PLUS][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_PLUS][OP_MORE] = PA_GREATER;
    table[OP_PLUS][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_PLUS][OP_EQUAL] = PA_GREATER;
    table[OP_PLUS][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_PLUS][OP_ID] = PA_LESS;
    table[OP_PLUS][OP_DOLLAR] = PA_GREATER;
    table[OP_PLUS][OP_DATATYPE] = PA_LESS;

    table[OP_MINUS][OP_PLUS] = PA_GREATER;
    table[OP_MINUS][OP_MINUS] = PA_GREATER;
    table[OP_MINUS][OP_MUL] = PA_LESS;    
    table[OP_MINUS][OP_DIV] = PA_LESS;
    table[OP_MINUS][OP_LBRAC] = PA_LESS;
    table[OP_MINUS][OP_RBRAC] = PA_GREATER;
    table[OP_MINUS][OP_LESS] = PA_GREATER;
    table[OP_MINUS][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_MINUS][OP_MORE] = PA_GREATER;
    table[OP_MINUS][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_MINUS][OP_EQUAL] = PA_GREATER;
    table[OP_MINUS][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_MINUS][OP_ID] = PA_LESS;
    table[OP_MINUS][OP_DOLLAR] = PA_GREATER;
    table[OP_MINUS][OP_DATATYPE] = PA_LESS;

    table[OP_MUL][OP_PLUS] = PA_GREATER;
    table[OP_MUL][OP_MINUS] = PA_GREATER;
    table[OP_MUL][OP_MUL] = PA_GREATER;    
    table[OP_MUL][OP_DIV] = PA_GREATER;
    table[OP_MUL][OP_LBRAC] = PA_LESS;
    table[OP_MUL][OP_RBRAC] = PA_GREATER;
    table[OP_MUL][OP_LESS] = PA_GREATER;
    table[OP_MUL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_MUL][OP_MORE] = PA_GREATER;
    table[OP_MUL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_MUL][OP_EQUAL] = PA_GREATER;
    table[OP_MUL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_MUL][OP_ID] = PA_LESS;
    table[OP_MUL][OP_DOLLAR] = PA_GREATER;
    table[OP_MUL][OP_DATATYPE] = PA_LESS;

    table[OP_DIV][OP_PLUS] = PA_GREATER;
    table[OP_DIV][OP_MINUS] = PA_GREATER;
    table[OP_DIV][OP_MUL] = PA_GREATER;    
    table[OP_DIV][OP_DIV] = PA_GREATER;
    table[OP_DIV][OP_LBRAC] = PA_LESS;
    table[OP_DIV][OP_RBRAC] = PA_GREATER;
    table[OP_DIV][OP_LESS] = PA_GREATER;
    table[OP_DIV][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_DIV][OP_MORE] = PA_GREATER;
    table[OP_DIV][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_DIV][OP_EQUAL] = PA_GREATER;
    table[OP_DIV][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_DIV][OP_ID] = PA_LESS;
    table[OP_DIV][OP_DOLLAR] = PA_GREATER;
    table[OP_DIV][OP_DATATYPE] = PA_LESS;

    table[OP_LBRAC][OP_PLUS] = PA_LESS;
    table[OP_LBRAC][OP_MINUS] = PA_LESS;
    table[OP_LBRAC][OP_MUL] = PA_LESS;    
    table[OP_LBRAC][OP_DIV] = PA_LESS;
    table[OP_LBRAC][OP_LBRAC] = PA_LESS;
    table[OP_LBRAC][OP_RBRAC] = PA_SHIFT;
    table[OP_LBRAC][OP_LESS] = PA_LESS;
    table[OP_LBRAC][OP_LESS_EQUAL] = PA_LESS;
    table[OP_LBRAC][OP_MORE] = PA_LESS;
    table[OP_LBRAC][OP_MORE_EQUAL] = PA_LESS;
    table[OP_LBRAC][OP_EQUAL] = PA_LESS;
    table[OP_LBRAC][OP_NOT_EQUAL] = PA_LESS;
    table[OP_LBRAC][OP_ID] = PA_LESS;
    table[OP_LBRAC][OP_DOLLAR] = PA_EMPTY;
    table[OP_LBRAC][OP_DATATYPE] = PA_LESS;

    table[OP_RBRAC][OP_PLUS] = PA_GREATER;
    table[OP_RBRAC][OP_MINUS] = PA_GREATER;
    table[OP_RBRAC][OP_MUL] = PA_GREATER;    
    table[OP_RBRAC][OP_DIV] = PA_GREATER;
    table[OP_RBRAC][OP_LBRAC] = PA_EMPTY;
    table[OP_RBRAC][OP_RBRAC] = PA_GREATER;
    table[OP_RBRAC][OP_LESS] = PA_GREATER;
    table[OP_RBRAC][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_RBRAC][OP_MORE] = PA_GREATER;
    table[OP_RBRAC][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_RBRAC][OP_EQUAL] = PA_GREATER;
    table[OP_RBRAC][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_RBRAC][OP_ID] = PA_EMPTY;
    table[OP_RBRAC][OP_DOLLAR] = PA_GREATER;
    table[OP_RBRAC][OP_DATATYPE] = PA_EMPTY;

    table[OP_LESS][OP_PLUS] = PA_LESS;
    table[OP_LESS][OP_MINUS] = PA_LESS;
    table[OP_LESS][OP_MUL] = PA_LESS;    
    table[OP_LESS][OP_DIV] = PA_LESS;
    table[OP_LESS][OP_LBRAC] = PA_LESS;
    table[OP_LESS][OP_RBRAC] = PA_GREATER;
    table[OP_LESS][OP_LESS] = PA_GREATER;
    table[OP_LESS][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_LESS][OP_MORE] = PA_GREATER;
    table[OP_LESS][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_LESS][OP_EQUAL] = PA_GREATER;
    table[OP_LESS][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_LESS][OP_ID] = PA_LESS;
    table[OP_LESS][OP_DOLLAR] = PA_GREATER;
    table[OP_LESS][OP_DATATYPE] = PA_LESS;

    table[OP_LESS_EQUAL][OP_PLUS] = PA_LESS;
    table[OP_LESS_EQUAL][OP_MINUS] = PA_LESS;
    table[OP_LESS_EQUAL][OP_MUL] = PA_LESS;    
    table[OP_LESS_EQUAL][OP_DIV] = PA_LESS;
    table[OP_LESS_EQUAL][OP_LBRAC] = PA_LESS;
    table[OP_LESS_EQUAL][OP_RBRAC] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_LESS] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_MORE] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_EQUAL] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_ID] = PA_LESS;
    table[OP_LESS_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_DATATYPE] = PA_LESS;

    table[OP_MORE][OP_PLUS] = PA_LESS;
    table[OP_MORE][OP_MINUS] = PA_LESS;
    table[OP_MORE][OP_MUL] = PA_LESS;    
    table[OP_MORE][OP_DIV] = PA_LESS;
    table[OP_MORE][OP_LBRAC] = PA_LESS;
    table[OP_MORE][OP_RBRAC] = PA_GREATER;
    table[OP_MORE][OP_LESS] = PA_GREATER;
    table[OP_MORE][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_MORE][OP_MORE] = PA_GREATER;
    table[OP_MORE][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_MORE][OP_EQUAL] = PA_GREATER;
    table[OP_MORE][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_MORE][OP_ID] = PA_LESS;
    table[OP_MORE][OP_DOLLAR] = PA_GREATER;
    table[OP_MORE][OP_DATATYPE] = PA_LESS;

    table[OP_MORE_EQUAL][OP_PLUS] = PA_LESS;
    table[OP_MORE_EQUAL][OP_MINUS] = PA_LESS;
    table[OP_MORE_EQUAL][OP_MUL] = PA_LESS;    
    table[OP_MORE_EQUAL][OP_DIV] = PA_LESS;
    table[OP_MORE_EQUAL][OP_LBRAC] = PA_LESS;
    table[OP_MORE_EQUAL][OP_RBRAC] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_LESS] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_MORE] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_EQUAL] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_ID] = PA_LESS;
    table[OP_MORE_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_DATATYPE] = PA_LESS;

    table[OP_EQUAL][OP_PLUS] = PA_LESS;
    table[OP_EQUAL][OP_MINUS] = PA_LESS;
    table[OP_EQUAL][OP_MUL] = PA_LESS;    
    table[OP_EQUAL][OP_DIV] = PA_LESS;
    table[OP_EQUAL][OP_LBRAC] = PA_LESS;
    table[OP_EQUAL][OP_RBRAC] = PA_GREATER;
    table[OP_EQUAL][OP_LESS] = PA_GREATER;
    table[OP_EQUAL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_EQUAL][OP_MORE] = PA_GREATER;
    table[OP_EQUAL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_EQUAL][OP_EQUAL] = PA_GREATER;
    table[OP_EQUAL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_EQUAL][OP_ID] = PA_LESS;
    table[OP_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_EQUAL][OP_DATATYPE] = PA_LESS;

    table[OP_NOT_EQUAL][OP_PLUS] = PA_LESS;
    table[OP_NOT_EQUAL][OP_MINUS] = PA_LESS;
    table[OP_NOT_EQUAL][OP_MUL] = PA_LESS;    
    table[OP_NOT_EQUAL][OP_DIV] = PA_LESS;
    table[OP_NOT_EQUAL][OP_LBRAC] = PA_LESS;
    table[OP_NOT_EQUAL][OP_RBRAC] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_LESS] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_MORE] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_EQUAL] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_ID] = PA_LESS;
    table[OP_NOT_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_DATATYPE] = PA_LESS;

    table[OP_ID][OP_PLUS] = PA_GREATER;
    table[OP_ID][OP_MINUS] = PA_GREATER;
    table[OP_ID][OP_MUL] = PA_GREATER;    
    table[OP_ID][OP_DIV] = PA_GREATER;
    table[OP_ID][OP_LBRAC] = PA_EMPTY;
    table[OP_ID][OP_RBRAC] = PA_GREATER;
    table[OP_ID][OP_LESS] = PA_GREATER;
    table[OP_ID][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_ID][OP_MORE] = PA_GREATER;
    table[OP_ID][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_ID][OP_EQUAL] = PA_GREATER;
    table[OP_ID][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_ID][OP_ID] = PA_EMPTY;
    table[OP_ID][OP_DOLLAR] = PA_GREATER;
    table[OP_ID][OP_DATATYPE] = PA_EMPTY;

    table[OP_DATATYPE][OP_PLUS] = PA_GREATER;
    table[OP_DATATYPE][OP_MINUS] = PA_GREATER;
    table[OP_DATATYPE][OP_MUL] = PA_GREATER;    
    table[OP_DATATYPE][OP_DIV] = PA_GREATER;
    table[OP_DATATYPE][OP_LBRAC] = PA_EMPTY;
    table[OP_DATATYPE][OP_RBRAC] = PA_GREATER;
    table[OP_DATATYPE][OP_LESS] = PA_GREATER;
    table[OP_DATATYPE][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_DATATYPE][OP_MORE] = PA_GREATER;
    table[OP_DATATYPE][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_DATATYPE][OP_EQUAL] = PA_GREATER;
    table[OP_DATATYPE][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_DATATYPE][OP_ID] = PA_EMPTY;
    table[OP_DATATYPE][OP_DOLLAR] = PA_GREATER;
    table[OP_DATATYPE][OP_DATATYPE] = PA_EMPTY;

    table[OP_DOLLAR][OP_PLUS] = PA_LESS;
    table[OP_DOLLAR][OP_MINUS] = PA_LESS;
    table[OP_DOLLAR][OP_MUL] = PA_LESS;    
    table[OP_DOLLAR][OP_DIV] = PA_LESS;
    table[OP_DOLLAR][OP_LBRAC] = PA_LESS;
    table[OP_DOLLAR][OP_RBRAC] = PA_EMPTY;
    table[OP_DOLLAR][OP_LESS] = PA_LESS;
    table[OP_DOLLAR][OP_LESS_EQUAL] = PA_LESS;
    table[OP_DOLLAR][OP_MORE] = PA_LESS;
    table[OP_DOLLAR][OP_MORE_EQUAL] = PA_LESS;
    table[OP_DOLLAR][OP_EQUAL] = PA_LESS;
    table[OP_DOLLAR][OP_NOT_EQUAL] = PA_LESS;
    table[OP_DOLLAR][OP_ID] = PA_LESS;
    table[OP_DOLLAR][OP_DOLLAR] = PA_EMPTY;
    table[OP_DOLLAR][OP_DATATYPE] = PA_LESS;
    return 0;
}

int rule1[3] = {OP_EXPRESSION, OP_PLUS, OP_EXPRESSION};          // E -> E + E
int rule2[3] = {OP_EXPRESSION, OP_MINUS, OP_EXPRESSION};         // E -> E - E
int rule3[3] = {OP_EXPRESSION, OP_MUL, OP_EXPRESSION};           // E -> E * E
int rule4[3] = {OP_EXPRESSION, OP_DIV, OP_EXPRESSION};           // E -> E / E

int rule5[3] = {OP_EXPRESSION, OP_LESS, OP_EXPRESSION};          // E -> E <  E
int rule6[3] = {OP_EXPRESSION, OP_LESS_EQUAL, OP_EXPRESSION};    // E -> E <= E
int rule7[3] = {OP_EXPRESSION, OP_MORE, OP_EXPRESSION};          // E -> E >  E
int rule8[3] = {OP_EXPRESSION, OP_MORE_EQUAL, OP_EXPRESSION};    // E -> E >= E
int rule9[3] = {OP_EXPRESSION, OP_EQUAL, OP_EXPRESSION};         // E -> E == E
int rule10[3] = {OP_EXPRESSION, OP_NOT_EQUAL, OP_EXPRESSION};    // E -> E != E

int rule11[3] = {OP_ID, 0, 0};                                   // E -> ID
int rule12[3] = {OP_RBRAC, OP_EXPRESSION, OP_LBRAC};             // E -> (E)
int rule13[3] = {OP_DATATYPE, 0, 0};                             // E -> i
