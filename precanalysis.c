/**
        ~~ IFJ-20 ~~
    @file   precanalysis.c
    @brief  implementation of precedence analysis automaton
    @author Martin Zaťovič
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "parser.h"
#include "scanner.h"
#include "generator.h"

extern symtableGlobalItem *actualFunc;
extern int isInFuncCall;
extern trAK *instr;
extern int dewit;
extern int ungot;
extern Token *tkns;

//-----------------INTEGER STACK FUNCTIONS-----------------//

bool intStackInit(is_t *intStack) {
    intStack->top = STACK_TOP;
    intStack->size = STACK_SIZE;
    intStack->iStack = NULL;
    intStack->iStack = malloc(STACK_SIZE * sizeof(int *));
    if(intStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return MALLOC_ERR;
    }
   if(intStack->iStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
    return true;
}

void intStackFree(is_t *intStack) {
    if (intStack != NULL) {
        for (int i = intStack->top; i > STACK_TOP; i--) {
            free(intStack->iStack[i]);
        }
        free(intStack->iStack);
        free(intStack);
    }
}

bool intStackIsFull(is_t *intStack) {
    if(intStack->top == intStack->size - 1)
        return true;
    return false;

}

bool intStackIsEmpty(is_t *intStack) {
    if(intStack->top == STACK_TOP)
        return true;
    return false;
}

bool intStackPush(is_t *intStack, int data) {
    if (intStackIsFull(intStack)) {
        int **helpPtr = NULL;

        intStack->size = intStack->size * 2;

        helpPtr = realloc(intStack->iStack, intStack->size*sizeof(int *));
        if (helpPtr == NULL) {
            fprintf(stderr, "Realloc failed to allocate memory");
            return false;
        }

        intStack->iStack = helpPtr;
        if(!intStackPush(intStack, data))
                return false;
    }
    else {

        int *tmp = NULL; 
        tmp = malloc(sizeof(int));
        *tmp = data;
        intStack->top++;
        intStack->iStack[intStack->top] = tmp;
    }
    return true;
}

int intStackPop(is_t *intStack) {
    if(!intStackIsEmpty(intStack)) {
        int ret = *intStack->iStack[intStack->top];
        free(intStack->iStack[intStack->top]);
        intStack->top--;
        return ret;
    }
    else
        return PA_ERR;
}

//-----------------SYMBOL STACK FUNCTIONS-----------------//

bool sInit (s_t* symStack) {
    symStack->top = STACK_TOP;
    symStack->size = STACK_SIZE;
    symStack->stack = NULL;
    //allocated pointers to structures
    symStack->stack = malloc(STACK_SIZE * sizeof(sElemType *));

    if (symStack->stack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
    for (int i = 0; i < STACK_SIZE; i++){//kuli was here
        //allocate structure itself
        symStack->stack[i] = malloc(sizeof(sElemType));
        symStack->stack[i]->name = NULL;
        symStack->stack[i]->paToken = NULL;
        
    }//kuli was NOT here -- doesn't seem to do anything
    
    return true;
}

void sFree(s_t *symStack) {
    if (symStack != NULL) {
        for (int i = symStack->top; i > STACK_TOP; i--) {
            free(symStack->stack[i]);
        }
        free(symStack->stack);
        free(symStack);
    }
}

bool sIsEmpty (s_t* symStack) {
    if(symStack->top == STACK_TOP)
        return true;
    return false;
}

bool sIsFull (s_t* symStack) {
    if(symStack->top == symStack->size - 1)
        return true;
    return false;
}

void sGetTop (s_t* symStack, sElemType *data) {
    if(sIsEmpty(symStack)) {
        // error
        return;
    }
    else {
        data->paType = (symStack->stack[symStack->top])->paType;
        strcpy((symStack->stack[symStack->top])->paToken->value, data->paToken->value);
        strcpy((symStack->stack[symStack->top])->name, data->name);
        data->paToken->type = (symStack->stack[symStack->top])->paToken->type;
        return;
    }
}

void sGetTopPointer(s_t *symStack, sElemType *data) {
    data->paType = (symStack->stack[symStack->top])->paType;
    data->paToken = (symStack->stack[symStack->top])->paToken;
    data->paToken->value = (symStack->stack[symStack->top])->paToken->value;
    data->name = (symStack->stack[symStack->top])->name;
    return;
}

void sPop (s_t* symStack) {
    if (sIsEmpty(symStack)) {
        return;
    }
    else {
        if( (symStack->stack[symStack->top])->paToken->value != NULL)
            free( (symStack->stack[symStack->top])->paToken->value);

        free((symStack->stack[symStack->top])->paToken);
        free((symStack->stack[symStack->top])->name);
        free(symStack->stack[symStack->top]);

        symStack->top--;
    }
}

void sPopPointer(s_t *symStack) {
    if (sIsEmpty(symStack)) {
        return;
    }
    else {
	free(symStack->stack[symStack->top]);
	symStack->stack[symStack->top] = NULL;
	symStack->top--;
    }
}

bool sPush (s_t* symStack, sElemType *newData) {
    if (sIsFull(symStack)) {
        sElemType **helpPtr = NULL;
        symStack->size = symStack->size + 10;
        
        helpPtr = realloc(symStack->stack, symStack->size*sizeof(sElemType *));
        
        if (helpPtr == NULL) {
                return false;
        }
        symStack->stack = helpPtr;
        if(!sPush(symStack, newData))
                return false;
    }
    else {
        sElemType *data = NULL;
        Token *paToken = NULL;
        
        if ((data = malloc(sizeof(sElemType))) == NULL)
        {
            fprintf(stderr, "Malloc failed to allocate memory");
            return false;
        }
        if ((paToken = malloc(sizeof(Token))) == NULL) {
            fprintf(stderr, "Malloc failed to allocate memory");
            free(data);
            return false;
        }
        data->paToken = paToken;
        
        data->paType = newData->paType;
        
        
        if(newData->paToken != NULL) {
            data->paToken->type = newData->paToken->type;

            if(newData->paToken->value != NULL && data->paToken->value == NULL) {
                if((data->paToken->value = malloc(strlen(newData->paToken->value) + 1)) == NULL) {
                    fprintf(stderr, "Malloc failed to allocate memory");
                    return false;
                }
                else {
                    strcpy(data->paToken->value, newData->paToken->value);
                }
            }
        }
        
        if(newData->name != NULL) {
            if(data->name == NULL) {
                if((data->name = malloc(ID_SIZE)) == NULL) {
                        fprintf(stderr, "Malloc failed to allocate memory");
                        return false;
                }
                else {
                    data->name = malloc(ID_SIZE);
                    strcpy(data->name, newData->name);
                }
            }
            else {
                data->name = malloc(ID_SIZE);
                strcpy(data->name, newData->name);
            }
        }
        symStack->top++;
        symStack->stack[symStack->top] = data; 
    }
    return true;
}

bool sPushPointer(s_t *symStack, sElemType *data) {
    if (sIsFull(symStack)) {
        sElemType **helpPtr = NULL;
        symStack->size += 100;

        helpPtr = realloc(symStack->stack, symStack->size*sizeof(sElemType *));
        if (helpPtr == NULL) {
            return false;
        }
        symStack->stack = helpPtr;
        if(!sPushPointer(symStack, data))
            return false;
    }
    else {
        sElemType *newData = NULL;
        if((newData = malloc(sizeof(sElemType))) == NULL)
            return false;
        newData->paType = data->paType;
        newData->paToken = data->paToken;
        newData->name = data->name;
        symStack->top++;
        symStack->stack[symStack->top] = newData;
    }
    return true;
}

void sElemInit(sElemType *data) {
    data->paToken->type = 0;
    data->paToken->value = NULL;
    data->paType = 0;
}

void sElemFree(sElemType *data, Token *paToken) {
    if (paToken != NULL) {
        if (paToken->value != NULL) {
            free(paToken->value);
        }
        free(paToken);
    }
    free(data);
}

bool sElemGetData(Token *token, sElemType *data) {
    data->paType = getPaType(token);
    data->paToken->type = token->type;

    if(data->paToken->value == NULL)
        data->paToken->value = malloc(strlen(token->value) + 1);

    if(data->paToken->value == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
    
    if(token->value != NULL) {
        strcpy(data->paToken->value, token->value);
    }
    return true;
}

bool sCopyUntilTerminal(s_t *src, s_t *dest, sElemType *tmp) {
    while((src->stack[src->top])->paType == OP_EXPRESSION || (src->stack[src->top])->paType == PA_LESS) {
        tmp->paType = 0;
        tmp->paToken = NULL;

        sGetTopPointer(src, tmp);

        if (!sPushPointer(dest, tmp))
            return false;

        sPopPointer(src);
    }
    return true;
}

bool sCopyAll(s_t *src, s_t *dest, sElemType *tmp) {
    while (!sIsEmpty(src)) {
        sGetTopPointer(src, tmp);
        
        if (!sPushPointer(dest, tmp))
            return false;

        sPopPointer(src);
    }
    return true;
}

int sFindFirstTerminal(s_t * symStack) {
    int tmp = symStack->top;

    while ((symStack->stack[tmp])->paType == OP_SEPARATOR || (symStack->stack[tmp])->paType == OP_EXPRESSION) {
        tmp--;
    }

    return (symStack->stack[tmp])->paType;
}

int getPaType(Token *token) {
    switch(token->type) {
        case KEYWORD:
            return OP_BOOL;

        case IDENTIFIER: {          // fun/ID
            Token t1,t2;
            getToken(&t1);
            if(t1.type == LEFT_ROUND_BRACKET) {
                getToken(&t2);
                if(t2.type == RIGHT_ROUND_BRACKET) {
                    return OP_FUN;      // fun()
                }
                else {
                    ungetToken(&t1);
                    ungetToken(&t2);
                    return OP_FUN;      // fun(args)
                }
            }
            if (ungot){
                if (!(tkns = realloc(tkns, sizeof(Token)*(ungot+1))))
                    return INTERNAL_ERROR;
                for (int oops = ungot; 0<oops; oops--){
                    tkns[oops] = tkns[oops-1];
                }
                tkns[0] = t1;
                ungot++;
            }
            else {
                ungetToken(&t1);
            }
            //printf("RETURNING OP_ID in getPaType");
            return OP_ID;   // else OP_ID
        }
        case INTEGER:{ 
            return OP_INTEGER;
        }
        case FLOAT: {
            return OP_FLOAT;
        }

        case STRING: {
            return OP_STRING;
        }
        case PLUS:
            return OP_PLUS;
        case MINUS:
            return OP_MINUS;
        case MUL:
            return OP_MUL;
        case DIV:
            return OP_DIV;
        case LESS:
            return OP_LESS;
        case LESS_OR_EQUAL:
            return OP_LESS_EQUAL;
        case GREATER:
            return OP_MORE;
        case GREATER_OR_EQUAL:
            return OP_MORE_EQUAL;
        case EQUAL:
            return OP_EQUAL;
        case NOT_EQUAL:
            return OP_NOT_EQUAL;
        case LEFT_ROUND_BRACKET:
            return OP_LBRAC;
        case RIGHT_ROUND_BRACKET: {
            if(isInFuncCall == 1)
                return OP_DOLLAR;   // special case when processing function arguments
            else
                return OP_RBRAC;
        }
        case NOT:
            return OP_NOT;
        case AND:
            return OP_AND;
        case OR:
            return OP_OR;
        case COMMA: {
            if(isInFuncCall == 1)
                return OP_DOLLAR;
            else
                return OP_DOLLAR;
            }
        case EOL_:
        case SEMICOLON:
        case LEFT_CURLY_BRACKET:
            return OP_DOLLAR;
    }
    return SYMBOL_NOT_RECOGNIZED;
}

int generateRule(int *rule, is_t *typeStack, int *lastFoundType, Token *teken, char *name, char *value1, char *value2) {
    int i = 0;
    bool found = false;

    sElemType *tmp = NULL;
    tmp = malloc(sizeof(sElemType));
    char *helpName = NULL;
    helpName = malloc(100);

    printf("\nRule found: %i %i %i\n", rule[0], rule[1],rule[2]);
    
    // compare with the set of rules in prectableandrules.c
    while(i < 20) {
        if(rule[0] == rules[i][0] && rule[1] == rules[i][1] && rule[2] == rules[i][2]) {
            found = true;
            break;
        }
        i++;
    }
    if(found) {
        char *nameGen = NULL;
        char *valueGen1 = NULL;
        char *valueGen2 = NULL;
        char *varrrGen = NULL;
        char *tekenGen = NULL;
         
        nameGen = malloc(100);
        valueGen1 = malloc(100);
        valueGen2 = malloc(100);
        varrrGen = malloc(100);
        tekenGen = malloc(100);
        
        nameGen = concat("LF@", name);
        if(value1)
            valueGen1 = concat("LF@", value1);

        if(value2)
            valueGen2 = concat("LF@", value2);

        if(i < 10 || i == 18 || i == 19) {  // two operand rules

            int type1 = intStackPop(typeStack);
            int type2 = intStackPop(typeStack);

            if(type1 == type2) {    // datatypes match
                if(i < 4 || i == 17 || i == 18) {   // +, -, *, /, &&, ||
                    intStackPush(typeStack, type1); // push operand datatype
                    *lastFoundType = type1;
                }
                else {      
                    type2 = BOOL;   // result of operation is boolean
                    intStackPush(typeStack, type2);
                    *lastFoundType = BOOL;
                }
               
                assemble("DEFVAR",nameGen,"","",instr);
                
                char *theInt = malloc(10);

                symtableItem *val1 = symtableItemGet(actualFunc->key,value1);
                if(val1 == NULL)
                    return SEM_ERR;

                if (val1){
                    char *varrr = malloc(11+strlen(value1));
                    if(!sprintf(theInt,"%d",val1->i))
                        return -INTERNAL_ERROR;
                    strcpy(varrr,value1);
                    varrr = strcat(varrr,"$");
                    varrr = strcat(varrr,theInt);
                    value1 = malloc(11+strlen(value1));
                    strcpy(value1,varrr);
                }

                symtableItem *val2 = symtableItemGet(actualFunc->key,value2);
                if(val2 == NULL)
                    return SEM_ERR;
                if (val2){
                    char *varrr = malloc(11+strlen(value2));
                    if(!sprintf(theInt,"%d",val2->i))
                        return -INTERNAL_ERROR;
                    strcpy(varrr,value2);
                    varrr = strcat(varrr,"$");
                    varrr = strcat(varrr,theInt);
                    value2 = malloc(11+strlen(value2));
                    strcpy(value2,varrr);
                }
                
                switch(i) {
                    case 0: {
                        if(type1 == STRING) {
                            assemble("CONCAT", nameGen, valueGen2, valueGen1, instr);
                        }
                        else {
                            assemble("ADD", nameGen, valueGen2, valueGen1, instr);
                        }
                        break;
                    }
                    case 1:
                        if(type1 == STRING)
                            return SEM_ERR_COMP_MINUS;
                        assemble("SUB", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 2:
                        if(type1 == STRING)
                            return SEM_ERR_COMP_MINUS;
                        assemble("MUL", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 3:
                        if(type1 == STRING)
                            return SEM_ERR_COMP_MINUS;
                        if(type1 == INTEGER){
                            assemble("DIV", nameGen, valueGen2, valueGen1, instr);
                        }
                        if(type1 == FLOAT) {
                            assemble("IDIV", nameGen, valueGen2, valueGen1, instr);
                        }
                        break;
                    case 4:
                        assemble("LT", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 5:
                        assemble("LT", nameGen, valueGen2, valueGen1, instr);
                        assemble("PUSHS", nameGen, "", "", instr);
                        assemble("EQ", nameGen, valueGen2, valueGen1, instr);
                        assemble("PUSHS", nameGen, "", "", instr);
                        assemble("ORS", nameGen, "", "", instr);
                        break;
                    case 6:
                        assemble("GT", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 7:
                        assemble("GT", nameGen, valueGen2, valueGen1, instr);
                        assemble("PUSHS", nameGen, "", "", instr);
                        assemble("EQ", nameGen, valueGen2, valueGen1, instr);
                        assemble("PUSHS", nameGen, "", "", instr);
                        assemble("ORS", nameGen, "", "", instr);
                        break;
                    case 8:
                        assemble("EQ", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 9:
                        assemble("EQ", nameGen, valueGen2, valueGen1, instr);
                        assemble("NOT", nameGen, nameGen, "", instr);
                        break;
                    case 17:
                    case 18:
                        assemble("AND", nameGen, valueGen2, valueGen1, instr);
                        break;
                    case 19:
                        assemble("PUSHS", valueGen2, "", "", instr);
                        assemble("PUSHS", valueGen1, "", "", instr);
                        assemble("ORS", nameGen, "", "", instr);
                        break;
                }
                free(nameGen);
                free(valueGen1);
                free(valueGen2);
                free(varrrGen);
                free(tekenGen);
                return i;
            }
            else {
                free(nameGen);
                free(valueGen1);
                free(valueGen2);
                free(varrrGen);
                free(tekenGen);
                return DIFFERENT_TYPES;
            }
            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);
            return i;
        }
        
        if(i == 10) {   // E -> ID
            symtableItem *item = symtableItemGet(actualFunc->key, teken->value);
            if(item == NULL)
                return SEM_ERR;
            int tmp = item->type;
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;

            char *varrr = malloc(11+strlen(item->key)); /// louda code
            char *theInt = malloc(10);                  ///

            if(!sprintf(theInt,"%d",item->i))
                return -INTERNAL_ERROR;
            strcpy(varrr,item->key);
            varrr = strcat(varrr,"$");
            varrr = strcat(varrr,theInt);               /// louda code end
            
            assemble("DEFVAR", nameGen,"","", instr);
            varrrGen = concat("LF@", varrr);
            assemble("MOVE", nameGen, varrrGen, "", instr);
            
            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);

            return i;
        }
        if(i == 11) {   // E -> (E)
            int tmp = intStackPop(typeStack);
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;
            
            assemble("DEFVAR", nameGen, "", "", instr);
            //               var   var
            assemble("MOVE", nameGen, valueGen2, "", instr);

            return i;
        } 
        if(i == 12) {   // E -> i(int)
            *lastFoundType = INTEGER;
            int tmp = INTEGER;
            intStackPush(typeStack, tmp);
             
            assemble("DEFVAR", nameGen, "", "", instr);

            tekenGen = concat("int@", teken->value); 
            assemble("MOVE", nameGen, tekenGen, "", instr);

            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);

            return i;
        }
        if(i == 13) {   // E -> i(float)
            *lastFoundType = FLOAT;
            int tmp = FLOAT; 
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", nameGen, "", "", instr);
            tekenGen = concat("float@", teken->value);
            assemble("MOVE", nameGen, tekenGen, "", instr);
            
            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);

            return i;
        }
        if(i == 14) {   // E -> i(string)
            *lastFoundType = STRING;
            int tmp = STRING; 
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", nameGen, "", "", instr);
            
            tekenGen = concat("string@", teken->value); //debug teken->value gets printed here as [ <main_arena+96> "" ]

            assemble("MOVE", nameGen, tekenGen, "", instr);

            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);

            return i;
        }
        if(i == 15) {
            *lastFoundType = NO_TYPE_OPERATION;
            return i;
        }
        if(i == 16) {   // E -> FUN E
            symtableGlobalItem *fun;
            if((fun = symtableItemGetGlobal(value2)) == NULL)
            if(fun->countRets == 1) {
                int tmp = fun->returns[0];
                *lastFoundType = tmp;
                intStackPush(typeStack, tmp);
            }
            else
                return SYNTAX_ERR;      // too many return values, max 1
            return i;
        }
        if(i == 17) {   //  E -> !E
            assemble("NOT", nameGen, nameGen, "", instr);
            int tmp = intStackPop(typeStack);
            tmp = BOOL;
            *lastFoundType = BOOL;
            intStackPush(typeStack, tmp);
            return i;
        }
        if(i == 20) {   // E -> i(bool)
            *lastFoundType = BOOL;
            int tmp = BOOL;
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", nameGen, "", "", instr);
            
            tekenGen = concat("bool@", teken->value);
            assemble("MOVE", nameGen, tekenGen, "", instr);

            free(nameGen);
            free(valueGen1);
            free(valueGen2);
            free(varrrGen);
            free(tekenGen);

            return i;
        }

    }
    return SYNTAX_ERR;
}

int sFindRule(s_t *mainStack, s_t *tmpStack, sElemType *tmpTerminal, is_t *typeStack, int * lastFoundType, char *name) {
    int i = 0;
    int usedRule;
    int ruleOnStack[3] = {0, 0, 0};
    bool isOp2Zero = false;
    
    char array[1]; 
    array[0] = '0';

    char *values[2] = {NULL, NULL};
    char *override = NULL;
    
    while ((mainStack->stack[mainStack->top])->paType != OP_SEPARATOR) {
        sGetTopPointer(mainStack, tmpTerminal);
        
        if (!sPushPointer(tmpStack, tmpTerminal))
            return PA_ERR;
        
        if(i == 0) {
            //printf("--------====heyo%i, %i, %i\n", mainStack->stack[mainStack->top]->paToken->value[0], mainStack->stack[mainStack->top]->paToken->value[1], mainStack->stack[mainStack->top]->paToken->value[2]);
            if( ((strcmp(mainStack->stack[mainStack->top]->paToken->value, array)) == 0) ||  ((strcmp(mainStack->stack[mainStack->top]->paToken->value, "0.0")) == 0))
                isOp2Zero = true;
 
            if(mainStack->stack[mainStack->top]->paType == OP_EXPRESSION)
                values[0] = mainStack->stack[mainStack->top]->name;
            else if(mainStack->stack[mainStack->top]->paType != OP_RBRAC) {
                values[0] = mainStack->stack[mainStack->top]->paToken->value;
            }
        }
        if(i == 1 && mainStack->stack[mainStack->top]->paType == OP_EXPRESSION) {
            override = mainStack->stack[mainStack->top]->name;
        }
        if(i == 1 && mainStack->stack[mainStack->top]->paType == OP_FUN) {
            override = mainStack->stack[mainStack->top]->paToken->value;
        }
        if(i == 2) {            
            if(mainStack->stack[mainStack->top]->paType == OP_EXPRESSION)
            {
               values[1] = mainStack->stack[mainStack->top]->name;
            }
            else
                values[1] = mainStack->stack[mainStack->top]->paToken->value;
        }
        sPopPointer(mainStack);
        i++;
    }
    
    if (tmpStack->top > 2)
        return PA_ERR;

    if(override != NULL) {
        values[0] = NULL;
        values[1] = override;
    }

    for (int i = tmpStack->top; i > STACK_TOP; i--)    // save the content of stack until PA_LESS is read
        ruleOnStack[i] = (tmpStack->stack[i])->paType;
    
    if(tmpStack->top == 0) {
        usedRule = generateRule(ruleOnStack, typeStack, lastFoundType, tmpStack->stack[0]->paToken, name, values[0], NULL);
    }
    else
        usedRule = generateRule(ruleOnStack, typeStack, lastFoundType, tmpStack->stack[0]->paToken, name, values[0], values[1]);
    
    //printf("\nsuedrule: %i", i);
    if(usedRule == 3 && isOp2Zero) {
        printf("-9vypisujem\n");
        return -9;
    }
    return usedRule;
}

int analyzePrecedence() {
    //printf("\nINA ZAVOLANA FUNKCIA\n");
 
    int action = 0;             // action determined by prectable
    int findRuleRet = 0;
    int lastFoundType = 0;      // type of most recently analyzed symbol or expression
    
    Token t;                    // currently analyzed token
    Token *paToken = NULL;             // helper Token
    sElemType *analyzedSymbol = NULL;  // currently analyzed Token and its paType in symbol structure
    
    sElemType *tmpSymbol = NULL;  // helper symbol struct
    tmpSymbol = malloc(sizeof(sElemType));

    char *name = NULL; 
    name = malloc(100);

    int precedentTable[25][25];
    pTableInit(precedentTable);

    // paToken struct init
    paToken = malloc(sizeof(Token));
    if(paToken == NULL) {        
        fprintf(stderr, "Malloc failed to allocate memory");
        return MALLOC_ERR;
    }    
    paToken->value = NULL;
    
    // analyzedSymbol struct init
    analyzedSymbol = malloc(sizeof(sElemType));
    analyzedSymbol->paToken = paToken;
    sElemInit(analyzedSymbol);
    analyzedSymbol->name = NULL;
    analyzedSymbol->name = malloc(ID_SIZE);
    
    // main symbol stack init
    s_t *mainStack = NULL;
    mainStack = malloc(sizeof(s_t));
    sInit(mainStack);
    tmpSymbol->paType = OP_DOLLAR;
    tmpSymbol->paToken = NULL;
    tmpSymbol->name = NULL;
    tmpSymbol->name = malloc(ID_SIZE); //DEBUG spot
    sPush(mainStack, tmpSymbol);   // push $ onto stack
    
    // tmp symbol stack init
    s_t *tmpStack = NULL;
    tmpStack = malloc(sizeof(s_t));
    if(tmpStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return MALLOC_ERR;
    }
    sInit(tmpStack);
   
    // integer stack for storage of types
    is_t *typeStack = NULL;
    typeStack = malloc(sizeof(is_t));
    if(typeStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return MALLOC_ERR;
    }
    intStackInit(typeStack);       
    
    getToken(&t);   // get the first token of expression
    analyzedSymbol->paToken->value = NULL;
    analyzedSymbol->paToken->value = malloc(strlen(t.value) + 1);

    sElemGetData(&t, analyzedSymbol);
    
    // implementation of precedence analysis automaton
    while(!(analyzedSymbol->paType == OP_DOLLAR && sFindFirstTerminal(mainStack) == OP_DOLLAR)) {
        //printf("\ntop stack: %i, mainterm: %i\n", sFindFirstTerminal(mainStack), analyzedSymbol->paType);
        // detrmine action based on input, top stack terminal and PA table
        action = precedentTable[sFindFirstTerminal(mainStack)][analyzedSymbol->paType];
        //printf("%i\n", action);
        switch(action) {
            case(PA_GREATER):
                name = generate_identifier();
                // determine rule on top of the stack
                findRuleRet = sFindRule(mainStack, tmpStack, tmpSymbol, typeStack, &lastFoundType, name);
                // rule does not exist -> syntactic error
                if(findRuleRet == SYNTAX_ERR)
                    return SYNTAX_ERR;

                // conflicting types of symbols -> semantic error
                if(findRuleRet == DIFFERENT_TYPES)
                    return SEM_ERR_COMP_MINUS;
                
                // pop the symbol on the top of the stack
                sPopPointer(mainStack);
                 
                tmpSymbol->paType = OP_EXPRESSION;
                tmpSymbol->paToken = (tmpStack->stack[tmpStack->top])->paToken;
                strcpy(tmpSymbol->name, name);
                
                // and replace it with Expression
                sPushPointer(mainStack, tmpSymbol);
                while (!sIsEmpty(tmpStack)) {
		    sPopPointer(tmpStack);
                }
                break;
            case(PA_LESS):
                // copy symbol on stack until the first terminal is found onto tmpStack
                sCopyUntilTerminal(mainStack, tmpStack, tmpSymbol);
                //push '<' onto the stack
                tmpSymbol->paToken = NULL;
		tmpSymbol->paType = OP_SEPARATOR;
                tmpSymbol->name = NULL;
		sPush(mainStack, tmpSymbol);
                // copy the symbols from tmpStack into mainStack /reversed order/
                sCopyAll(tmpStack, mainStack, tmpSymbol);
                // pushes the symbol just where it should be :3
                sPush(mainStack, analyzedSymbol);
                
                // get next symbol to analyze
                getToken(&t);
                break;
            case(PA_SHIFT):
                sPush(mainStack, analyzedSymbol);
                getToken(&t);
                break;
            case(PA_EMPTY):
                // syntactic error
                return SYNTAX_ERR;
            }
        // updates the analyzed symbol if new Token was gotten
        sElemGetData(&t, analyzedSymbol);
        if(analyzedSymbol->paType == SYMBOL_NOT_RECOGNIZED) {
            return SYNTAX_ERR;
        }
    }
    
    char *tmpSymbolGenUniq = concat("LF@",tmpSymbol->name);//kuli was here
    assemble("PUSHS", tmpSymbolGenUniq, "", "", instr);//kuli was here
    ungetToken(&t);
    free(tmpSymbolGenUniq);//kuli was here
    return lastFoundType;
}
