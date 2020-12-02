/**
        ~~ IFJ-20 ~~
    @file   prectableandrules.c
    @brief  contains precedence table and rules used by PA
    @authoe Martin Zatovic
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "scanner.h"

bool pTableInit(int table[25][25]) {
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
    table[OP_PLUS][OP_NOT] = PA_GREATER;
    table[OP_PLUS][OP_AND] = PA_GREATER;
    table[OP_PLUS][OP_OR] = PA_GREATER;
    table[OP_PLUS][OP_FUN] = PA_LESS;
    table[OP_PLUS][OP_COMMA] = PA_GREATER;
    table[OP_PLUS][OP_ID] = PA_LESS;
    table[OP_PLUS][OP_DOLLAR] = PA_GREATER;
    table[OP_PLUS][OP_INTEGER] = PA_LESS;
    table[OP_PLUS][OP_FLOAT] = PA_LESS;
    table[OP_PLUS][OP_STRING] = PA_LESS;  
    table[OP_PLUS][OP_BOOL] = PA_LESS;

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
    table[OP_MINUS][OP_NOT] = PA_GREATER;
    table[OP_MINUS][OP_AND] = PA_GREATER;
    table[OP_MINUS][OP_OR] = PA_GREATER;
    table[OP_MINUS][OP_FUN] = PA_LESS;
    table[OP_MINUS][OP_COMMA] = PA_GREATER;
    table[OP_MINUS][OP_ID] = PA_LESS;
    table[OP_MINUS][OP_DOLLAR] = PA_GREATER;
    table[OP_MINUS][OP_INTEGER] = PA_LESS;
    table[OP_MINUS][OP_FLOAT] = PA_LESS;
    table[OP_MINUS][OP_STRING] = PA_LESS;  
    table[OP_MINUS][OP_BOOL] = PA_LESS;

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
    table[OP_MUL][OP_NOT] = PA_GREATER;
    table[OP_MUL][OP_AND] = PA_GREATER;
    table[OP_MUL][OP_OR] = PA_GREATER;
    table[OP_MUL][OP_FUN] = PA_LESS;
    table[OP_MUL][OP_COMMA] = PA_GREATER;
    table[OP_MUL][OP_ID] = PA_LESS;
    table[OP_MUL][OP_DOLLAR] = PA_GREATER;
    table[OP_MUL][OP_INTEGER] = PA_LESS;
    table[OP_MUL][OP_FLOAT] = PA_LESS;
    table[OP_MUL][OP_STRING] = PA_LESS;  
    table[OP_MUL][OP_BOOL] = PA_LESS;

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
    table[OP_DIV][OP_NOT] = PA_GREATER;
    table[OP_DIV][OP_AND] = PA_GREATER;
    table[OP_DIV][OP_OR] = PA_GREATER;
    table[OP_DIV][OP_FUN] = PA_LESS;
    table[OP_DIV][OP_COMMA] = PA_GREATER;
    table[OP_DIV][OP_ID] = PA_LESS;
    table[OP_DIV][OP_DOLLAR] = PA_GREATER;
    table[OP_DIV][OP_INTEGER] = PA_LESS;
    table[OP_DIV][OP_FLOAT] = PA_LESS;
    table[OP_DIV][OP_STRING] = PA_LESS;  
    table[OP_DIV][OP_BOOL] = PA_LESS;

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
    table[OP_LBRAC][OP_NOT] = PA_LESS;
    table[OP_LBRAC][OP_AND] = PA_LESS;
    table[OP_LBRAC][OP_OR] = PA_LESS;
    table[OP_LBRAC][OP_FUN] = PA_LESS;
    table[OP_LBRAC][OP_COMMA] = PA_GREATER;
    table[OP_LBRAC][OP_ID] = PA_LESS;
    table[OP_LBRAC][OP_DOLLAR] = PA_EMPTY;
    table[OP_LBRAC][OP_INTEGER] = PA_LESS;
    table[OP_LBRAC][OP_FLOAT] = PA_LESS;
    table[OP_LBRAC][OP_STRING] = PA_LESS;  
    table[OP_LBRAC][OP_BOOL] = PA_LESS;

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
    table[OP_RBRAC][OP_NOT] = PA_GREATER;
    table[OP_RBRAC][OP_AND] = PA_GREATER;
    table[OP_RBRAC][OP_OR] = PA_GREATER;
    table[OP_RBRAC][OP_FUN] = PA_LESS;
    table[OP_RBRAC][OP_COMMA] = PA_GREATER;
    table[OP_RBRAC][OP_ID] = PA_EMPTY;
    table[OP_RBRAC][OP_DOLLAR] = PA_GREATER;
    table[OP_RBRAC][OP_INTEGER] = PA_EMPTY;
    table[OP_RBRAC][OP_FLOAT] = PA_EMPTY;
    table[OP_RBRAC][OP_STRING] = PA_EMPTY;  
    table[OP_RBRAC][OP_BOOL] = PA_EMPTY;

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
    table[OP_LESS][OP_NOT] = PA_GREATER;
    table[OP_LESS][OP_AND] = PA_GREATER;
    table[OP_LESS][OP_OR] = PA_GREATER;
    table[OP_LESS][OP_FUN] = PA_LESS;
    table[OP_LESS][OP_COMMA] = PA_GREATER;
    table[OP_LESS][OP_ID] = PA_LESS;
    table[OP_LESS][OP_DOLLAR] = PA_GREATER;
    table[OP_LESS][OP_INTEGER] = PA_LESS;
    table[OP_LESS][OP_FLOAT] = PA_LESS;
    table[OP_LESS][OP_STRING] = PA_LESS;  
    table[OP_LESS][OP_BOOL] = PA_LESS;

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
    table[OP_LESS_EQUAL][OP_NOT] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_AND] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_OR] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_FUN] = PA_LESS;
    table[OP_LESS_EQUAL][OP_COMMA] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_ID] = PA_LESS;
    table[OP_LESS_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_LESS_EQUAL][OP_INTEGER] = PA_LESS;
    table[OP_LESS_EQUAL][OP_FLOAT] = PA_LESS;
    table[OP_LESS_EQUAL][OP_STRING] = PA_LESS;  
    table[OP_LESS_EQUAL][OP_BOOL] = PA_LESS;

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
    table[OP_MORE][OP_NOT] = PA_GREATER;
    table[OP_MORE][OP_AND] = PA_GREATER;
    table[OP_MORE][OP_OR] = PA_GREATER;
    table[OP_MORE][OP_FUN] = PA_LESS;
    table[OP_MORE][OP_COMMA] = PA_GREATER;
    table[OP_MORE][OP_ID] = PA_LESS;
    table[OP_MORE][OP_DOLLAR] = PA_GREATER;
    table[OP_MORE][OP_INTEGER] = PA_LESS;
    table[OP_MORE][OP_FLOAT] = PA_LESS;
    table[OP_MORE][OP_STRING] = PA_LESS;  
    table[OP_MORE][OP_BOOL] = PA_LESS;

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
    table[OP_MORE_EQUAL][OP_NOT] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_AND] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_OR] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_FUN] = PA_LESS;
    table[OP_MORE_EQUAL][OP_COMMA] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_ID] = PA_LESS;
    table[OP_MORE_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_MORE_EQUAL][OP_INTEGER] = PA_LESS;
    table[OP_MORE_EQUAL][OP_FLOAT] = PA_LESS;
    table[OP_MORE_EQUAL][OP_STRING] = PA_LESS;  
    table[OP_MORE_EQUAL][OP_BOOL] = PA_LESS;

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
    table[OP_EQUAL][OP_NOT] = PA_GREATER;
    table[OP_EQUAL][OP_AND] = PA_GREATER;
    table[OP_EQUAL][OP_OR] = PA_GREATER;
    table[OP_EQUAL][OP_FUN] = PA_LESS;
    table[OP_EQUAL][OP_COMMA] = PA_GREATER;
    table[OP_EQUAL][OP_ID] = PA_LESS;
    table[OP_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_EQUAL][OP_INTEGER] = PA_LESS;
    table[OP_EQUAL][OP_FLOAT] = PA_LESS;
    table[OP_EQUAL][OP_STRING] = PA_LESS;  
    table[OP_EQUAL][OP_BOOL] = PA_LESS;

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
    table[OP_NOT_EQUAL][OP_NOT] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_AND] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_OR] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_FUN] = PA_LESS;
    table[OP_NOT_EQUAL][OP_COMMA] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_ID] = PA_LESS;
    table[OP_NOT_EQUAL][OP_DOLLAR] = PA_GREATER;
    table[OP_NOT_EQUAL][OP_INTEGER] = PA_LESS;
    table[OP_NOT_EQUAL][OP_FLOAT] = PA_LESS;
    table[OP_NOT_EQUAL][OP_STRING] = PA_LESS;  
    table[OP_NOT_EQUAL][OP_BOOL] = PA_LESS;

    table[OP_FUN][OP_PLUS] = PA_GREATER;
    table[OP_FUN][OP_MINUS] = PA_GREATER;
    table[OP_FUN][OP_MUL] = PA_GREATER;    
    table[OP_FUN][OP_DIV] = PA_GREATER;
    table[OP_FUN][OP_LBRAC] = PA_LESS;
    table[OP_FUN][OP_RBRAC] = PA_GREATER;
    table[OP_FUN][OP_LESS] = PA_GREATER;
    table[OP_FUN][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_FUN][OP_MORE] = PA_GREATER;
    table[OP_FUN][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_FUN][OP_EQUAL] = PA_GREATER;
    table[OP_FUN][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_FUN][OP_NOT] = PA_GREATER;
    table[OP_FUN][OP_AND] = PA_GREATER;
    table[OP_FUN][OP_OR] = PA_GREATER;
    table[OP_FUN][OP_FUN] = PA_LESS;
    table[OP_FUN][OP_COMMA] = PA_GREATER;
    table[OP_FUN][OP_FUN] = PA_LESS;
    table[OP_FUN][OP_COMMA] = PA_GREATER;   
    table[OP_FUN][OP_ID] = PA_LESS;
    table[OP_FUN][OP_DOLLAR] = PA_GREATER;
    table[OP_FUN][OP_INTEGER] = PA_LESS;
    table[OP_FUN][OP_FLOAT] = PA_LESS;
    table[OP_FUN][OP_STRING] = PA_LESS;  
    table[OP_FUN][OP_BOOL] = PA_LESS;

    table[OP_COMMA][OP_PLUS] = PA_LESS;
    table[OP_COMMA][OP_MINUS] = PA_LESS;
    table[OP_COMMA][OP_MUL] = PA_LESS;    
    table[OP_COMMA][OP_DIV] = PA_LESS;
    table[OP_COMMA][OP_LBRAC] = PA_LESS;
    table[OP_COMMA][OP_RBRAC] = PA_GREATER;
    table[OP_COMMA][OP_LESS] = PA_GREATER;
    table[OP_COMMA][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_COMMA][OP_MORE] = PA_GREATER;
    table[OP_COMMA][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_COMMA][OP_EQUAL] = PA_GREATER;
    table[OP_COMMA][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_COMMA][OP_NOT] = PA_GREATER;
    table[OP_COMMA][OP_AND] = PA_GREATER;
    table[OP_COMMA][OP_OR] = PA_GREATER;
    table[OP_COMMA][OP_FUN] = PA_LESS;
    table[OP_COMMA][OP_COMMA] = PA_GREATER;
    table[OP_COMMA][OP_FUN] = PA_LESS;
    table[OP_COMMA][OP_COMMA] = PA_LESS;   
    table[OP_COMMA][OP_ID] = PA_LESS;
    table[OP_COMMA][OP_DOLLAR] = PA_GREATER;
    table[OP_COMMA][OP_INTEGER] = PA_LESS;
    table[OP_COMMA][OP_FLOAT] = PA_LESS;
    table[OP_COMMA][OP_STRING] = PA_LESS;  
    table[OP_COMMA][OP_BOOL] = PA_LESS;
 
    table[OP_NOT][OP_PLUS] = PA_LESS;
    table[OP_NOT][OP_MINUS] = PA_LESS;
    table[OP_NOT][OP_MUL] = PA_LESS;
    table[OP_NOT][OP_DIV] = PA_LESS;
    table[OP_NOT][OP_LBRAC] = PA_LESS;
    table[OP_NOT][OP_RBRAC] = PA_GREATER;
    table[OP_NOT][OP_LESS] = PA_LESS;
    table[OP_NOT][OP_LESS_EQUAL] = PA_LESS;
    table[OP_NOT][OP_MORE] = PA_LESS;
    table[OP_NOT][OP_MORE_EQUAL] = PA_LESS;
    table[OP_NOT][OP_EQUAL] = PA_LESS;
    table[OP_NOT][OP_NOT_EQUAL] = PA_LESS;
    table[OP_NOT][OP_NOT] = PA_GREATER;
    table[OP_NOT][OP_AND] = PA_GREATER;
    table[OP_NOT][OP_OR] = PA_GREATER;
    table[OP_NOT][OP_FUN] = PA_LESS;
    table[OP_NOT][OP_COMMA] = PA_GREATER;
    table[OP_NOT][OP_ID] = PA_LESS;
    table[OP_NOT][OP_DOLLAR] = PA_GREATER;
    table[OP_NOT][OP_INTEGER] = PA_LESS;
    table[OP_NOT][OP_FLOAT] = PA_LESS;
    table[OP_NOT][OP_STRING] = PA_LESS; 
    table[OP_NOT][OP_BOOL] = PA_LESS;

    table[OP_AND][OP_PLUS] = PA_LESS;
    table[OP_AND][OP_MINUS] = PA_LESS;
    table[OP_AND][OP_MUL] = PA_LESS;    
    table[OP_AND][OP_DIV] = PA_LESS;
    table[OP_AND][OP_LBRAC] = PA_LESS;
    table[OP_AND][OP_RBRAC] = PA_GREATER;
    table[OP_AND][OP_LESS] = PA_GREATER;
    table[OP_AND][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_AND][OP_MORE] = PA_GREATER;
    table[OP_AND][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_AND][OP_EQUAL] = PA_GREATER;
    table[OP_AND][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_AND][OP_NOT] = PA_GREATER;
    table[OP_AND][OP_AND] = PA_GREATER;
    table[OP_AND][OP_OR] = PA_GREATER;
    table[OP_AND][OP_FUN] = PA_LESS;
    table[OP_AND][OP_COMMA] = PA_GREATER;
    table[OP_AND][OP_FUN] = PA_LESS;
    table[OP_AND][OP_COMMA] = PA_LESS;   
    table[OP_AND][OP_ID] = PA_LESS;
    table[OP_AND][OP_DOLLAR] = PA_GREATER;
    table[OP_AND][OP_INTEGER] = PA_LESS;
    table[OP_AND][OP_FLOAT] = PA_LESS;
    table[OP_AND][OP_STRING] = PA_LESS;  
    table[OP_AND][OP_BOOL] = PA_LESS;

    table[OP_OR][OP_PLUS] = PA_LESS;
    table[OP_OR][OP_MINUS] = PA_LESS;
    table[OP_OR][OP_MUL] = PA_LESS;    
    table[OP_OR][OP_DIV] = PA_LESS;
    table[OP_OR][OP_LBRAC] = PA_LESS;
    table[OP_OR][OP_RBRAC] = PA_GREATER;
    table[OP_OR][OP_LESS] = PA_GREATER;
    table[OP_OR][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_OR][OP_MORE] = PA_GREATER;
    table[OP_OR][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_OR][OP_EQUAL] = PA_GREATER;
    table[OP_OR][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_OR][OP_NOT] = PA_GREATER;
    table[OP_OR][OP_AND] = PA_GREATER;
    table[OP_OR][OP_OR] = PA_GREATER;
    table[OP_OR][OP_FUN] = PA_LESS;
    table[OP_OR][OP_COMMA] = PA_GREATER;
    table[OP_OR][OP_FUN] = PA_LESS;
    table[OP_OR][OP_COMMA] = PA_LESS;   
    table[OP_OR][OP_ID] = PA_LESS;
    table[OP_OR][OP_DOLLAR] = PA_GREATER;
    table[OP_OR][OP_INTEGER] = PA_LESS;
    table[OP_OR][OP_FLOAT] = PA_LESS;
    table[OP_OR][OP_STRING] = PA_LESS;  
    table[OP_OR][OP_BOOL] = PA_LESS;

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
    table[OP_ID][OP_NOT] = PA_EMPTY;
    table[OP_ID][OP_AND] = PA_GREATER;
    table[OP_ID][OP_OR] = PA_GREATER;
    table[OP_ID][OP_FUN] = PA_LESS;
    table[OP_ID][OP_COMMA] = PA_GREATER;
    table[OP_ID][OP_ID] = PA_EMPTY;
    table[OP_ID][OP_DOLLAR] = PA_GREATER;
    table[OP_ID][OP_INTEGER] = PA_EMPTY;
    table[OP_ID][OP_FLOAT] = PA_EMPTY;
    table[OP_ID][OP_STRING] = PA_EMPTY;  
    table[OP_ID][OP_BOOL] = PA_EMPTY;

    table[OP_INTEGER][OP_PLUS] = PA_GREATER;
    table[OP_INTEGER][OP_MINUS] = PA_GREATER;
    table[OP_INTEGER][OP_MUL] = PA_GREATER;    
    table[OP_INTEGER][OP_DIV] = PA_GREATER;
    table[OP_INTEGER][OP_LBRAC] = PA_EMPTY;
    table[OP_INTEGER][OP_RBRAC] = PA_GREATER;
    table[OP_INTEGER][OP_LESS] = PA_GREATER;
    table[OP_INTEGER][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_INTEGER][OP_MORE] = PA_GREATER;
    table[OP_INTEGER][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_INTEGER][OP_EQUAL] = PA_GREATER;
    table[OP_INTEGER][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_INTEGER][OP_NOT] = PA_EMPTY;
    table[OP_INTEGER][OP_AND] = PA_GREATER;
    table[OP_INTEGER][OP_OR] = PA_GREATER;
    table[OP_INTEGER][OP_FUN] = PA_GREATER;
    table[OP_INTEGER][OP_COMMA] = PA_GREATER;
    table[OP_INTEGER][OP_ID] = PA_EMPTY;
    table[OP_INTEGER][OP_DOLLAR] = PA_GREATER;
    table[OP_INTEGER][OP_INTEGER] = PA_EMPTY;
    table[OP_INTEGER][OP_FLOAT] = PA_EMPTY;
    table[OP_INTEGER][OP_STRING] = PA_EMPTY; 
    table[OP_INTEGER][OP_BOOL] = PA_EMPTY;

    table[OP_FLOAT][OP_PLUS] = PA_GREATER;
    table[OP_FLOAT][OP_MINUS] = PA_GREATER;
    table[OP_FLOAT][OP_MUL] = PA_GREATER;    
    table[OP_FLOAT][OP_DIV] = PA_GREATER;
    table[OP_FLOAT][OP_LBRAC] = PA_EMPTY;
    table[OP_FLOAT][OP_RBRAC] = PA_GREATER;
    table[OP_FLOAT][OP_LESS] = PA_GREATER;
    table[OP_FLOAT][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_FLOAT][OP_MORE] = PA_GREATER;
    table[OP_FLOAT][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_FLOAT][OP_EQUAL] = PA_GREATER;
    table[OP_FLOAT][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_FLOAT][OP_NOT] = PA_EMPTY;
    table[OP_FLOAT][OP_AND] = PA_GREATER;
    table[OP_FLOAT][OP_OR] = PA_GREATER;
    table[OP_FLOAT][OP_FUN] = PA_GREATER;
    table[OP_FLOAT][OP_COMMA] = PA_GREATER;
    table[OP_FLOAT][OP_ID] = PA_EMPTY;
    table[OP_FLOAT][OP_DOLLAR] = PA_GREATER;
    table[OP_FLOAT][OP_INTEGER] = PA_EMPTY;
    table[OP_FLOAT][OP_FLOAT] = PA_EMPTY;
    table[OP_FLOAT][OP_STRING] = PA_EMPTY; 
    table[OP_FLOAT][OP_BOOL] = PA_EMPTY;

    table[OP_STRING][OP_PLUS] = PA_GREATER;
    table[OP_STRING][OP_MINUS] = PA_GREATER;
    table[OP_STRING][OP_MUL] = PA_GREATER;    
    table[OP_STRING][OP_DIV] = PA_GREATER;
    table[OP_STRING][OP_LBRAC] = PA_EMPTY;
    table[OP_STRING][OP_RBRAC] = PA_GREATER;
    table[OP_STRING][OP_LESS] = PA_GREATER;
    table[OP_STRING][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_STRING][OP_MORE] = PA_GREATER;
    table[OP_STRING][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_STRING][OP_EQUAL] = PA_GREATER;
    table[OP_STRING][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_STRING][OP_NOT] = PA_EMPTY;
    table[OP_STRING][OP_AND] = PA_GREATER;
    table[OP_STRING][OP_OR] = PA_GREATER;
    table[OP_STRING][OP_FUN] = PA_GREATER;
    table[OP_STRING][OP_COMMA] = PA_GREATER;
    table[OP_STRING][OP_ID] = PA_EMPTY;
    table[OP_STRING][OP_DOLLAR] = PA_GREATER;
    table[OP_STRING][OP_INTEGER] = PA_EMPTY;
    table[OP_STRING][OP_FLOAT] = PA_EMPTY;
    table[OP_STRING][OP_STRING] = PA_EMPTY; 
    table[OP_STRING][OP_BOOL] = PA_EMPTY;
    
    table[OP_BOOL][OP_PLUS] = PA_GREATER;
    table[OP_BOOL][OP_MINUS] = PA_GREATER;
    table[OP_BOOL][OP_MUL] = PA_GREATER;    
    table[OP_BOOL][OP_DIV] = PA_GREATER;
    table[OP_BOOL][OP_LBRAC] = PA_EMPTY;
    table[OP_BOOL][OP_RBRAC] = PA_GREATER;
    table[OP_BOOL][OP_LESS] = PA_GREATER;
    table[OP_BOOL][OP_LESS_EQUAL] = PA_GREATER;
    table[OP_BOOL][OP_MORE] = PA_GREATER;
    table[OP_BOOL][OP_MORE_EQUAL] = PA_GREATER;
    table[OP_BOOL][OP_EQUAL] = PA_GREATER;
    table[OP_BOOL][OP_NOT_EQUAL] = PA_GREATER;
    table[OP_BOOL][OP_NOT] = PA_GREATER;
    table[OP_BOOL][OP_AND] = PA_GREATER;
    table[OP_BOOL][OP_OR] = PA_GREATER;
    table[OP_BOOL][OP_FUN] = PA_GREATER;
    table[OP_BOOL][OP_COMMA] = PA_GREATER;
    table[OP_BOOL][OP_ID] = PA_EMPTY;
    table[OP_BOOL][OP_DOLLAR] = PA_GREATER;
    table[OP_BOOL][OP_INTEGER] = PA_EMPTY;
    table[OP_BOOL][OP_FLOAT] = PA_EMPTY;
    table[OP_BOOL][OP_STRING] = PA_EMPTY; 
    table[OP_BOOL][OP_BOOL] = PA_EMPTY;

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
    table[OP_DOLLAR][OP_NOT] = PA_LESS;
    table[OP_DOLLAR][OP_AND] = PA_LESS;
    table[OP_DOLLAR][OP_OR] = PA_LESS;
    table[OP_DOLLAR][OP_FUN] = PA_LESS;
    table[OP_DOLLAR][OP_COMMA] = PA_LESS;
    table[OP_DOLLAR][OP_ID] = PA_LESS;
    table[OP_DOLLAR][OP_DOLLAR] = PA_EMPTY; 
    table[OP_DOLLAR][OP_INTEGER] = PA_LESS;
    table[OP_DOLLAR][OP_FLOAT] = PA_LESS;
    table[OP_DOLLAR][OP_STRING] = PA_LESS; 
    table[OP_DOLLAR][OP_BOOL] = PA_LESS;
   
    return true;
}

int rules[25][25] = {
    {OP_EXPRESSION, OP_PLUS, OP_EXPRESSION},          // E -> E + E
    {OP_EXPRESSION, OP_MINUS, OP_EXPRESSION},         // E -> E - E
    {OP_EXPRESSION, OP_MUL, OP_EXPRESSION},           // E -> E * E
    {OP_EXPRESSION, OP_DIV, OP_EXPRESSION},           // E -> E / E
    {OP_EXPRESSION, OP_LESS, OP_EXPRESSION},          // E -> E <  E
    {OP_EXPRESSION, OP_LESS_EQUAL, OP_EXPRESSION},    // E -> E <= E
    {OP_EXPRESSION, OP_MORE, OP_EXPRESSION},          // E -> E >  E
    {OP_EXPRESSION, OP_MORE_EQUAL, OP_EXPRESSION},    // E -> E >= E
    {OP_EXPRESSION, OP_EQUAL, OP_EXPRESSION},         // E -> E == E
    {OP_EXPRESSION, OP_NOT_EQUAL, OP_EXPRESSION},    // E -> E != E

    {OP_ID, 0, 0},                                   // E -> ID
    {OP_RBRAC, OP_EXPRESSION, OP_LBRAC},             // E -> (E)

    {OP_INTEGER, 0, 0},                             // E -> i
    {OP_FLOAT, 0, 0},                             // E -> i
    {OP_STRING, 0, 0},                             // E -> i

    //{OP_EXPRESSION, OP_COMMA, OP_EXPRESSION},        // E -> E , E
    {OP_FUN, OP_EXPRESSION, 0},                     // E -> FUNCTION

    {OP_NOT, OP_EXPRESSION, 0},
    {OP_EXPRESSION, OP_AND, OP_EXPRESSION},
    {OP_EXPRESSION, OP_OR, OP_EXPRESSION},
    {OP_BOOL, 0, 0},
};
