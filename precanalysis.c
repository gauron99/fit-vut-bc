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

//-----------------INTEGER STACK FUNCTIONS-----------------//

bool intStackInit(is_t *intStack) {
    intStack->top = STACK_TOP;
    intStack->size = STACK_SIZE;
    intStack->iStack = malloc(STACK_SIZE * sizeof(int *));
    if(intStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return -908;
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
        int *tmp = malloc(sizeof(int));
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
        return -1;
}

//-----------------SYMBOL STACK FUNCTIONS-----------------//

bool sInit (s_t* symStack) {
    symStack->top = STACK_TOP;
    symStack->size = STACK_SIZE;
    symStack->stack = malloc(STACK_SIZE * sizeof(sElemType *));
    if (symStack->stack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
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
    //printf("\n-------------sgettoppointer value : %s", (symStack->stack[symStack->top])->paToken->value);
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
        
        /*if(newData->paToken != NULL) {
            if(newData->paToken->value != NULL)
                printf("\nVALUE: %s", newData->paToken->value);
        }*/
        
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
        sElemType *newData;
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
    //printf("eleminit\n");
    data->paToken->type = 0;
    data->paToken->value = NULL;
    data->paType = 0;
    //printf("eleminitreturn");
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
        //printf("SKOPIROVAL SOM VALUE : %s\n", data->paToken->value);
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

    while ((symStack->stack[tmp])->paType == OP_LESS || (symStack->stack[tmp])->paType == OP_EXPRESSION) {
        tmp--;
    }

    return (symStack->stack[tmp])->paType;
}

int getPaType(Token *token) {
    switch(token->type) {
        case KEYWORD:
            return OP_BOOL;

        case IDENTIFIER: {          // fun/ID
            /*printf("HALOHOHOHO");
            if(token->value != NULL) {
                printf("\nPATYPEVALUE: %s\n", token->value);
            }*/
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
            ungetToken(&t1);
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
        case COMMA:
            return OP_DOLLAR;
        case EOL_:
        case SEMICOLON:
        case LEFT_CURLY_BRACKET:
            return OP_DOLLAR;
    }
    return -1;
}

int generateRule(int *rule, is_t *typeStack, int *lastFoundType, Token *teken, char *name, char *value1, char *value2) {
    int i = 0;
    bool found = false;

    sElemType *tmp = malloc(sizeof(sElemType));
    char *helpName = malloc(100);

    //printf("\nRule found: %i %i %i\n", rule[0], rule[1],rule[2]);
    
    // compare with the set of rules in prectableandrules.c
    while(i < 20) {
        if(rule[0] == rules[i][0] && rule[1] == rules[i][1] && rule[2] == rules[i][2]) {
            found = true;
            break;
        }
        i++;
    }
    
    if(found) {
        //printf("CHECK 2, i = %i\n", i);
        if(i < 10 || i == 17 || i == 18) {  // two operand rules

            int type1 = intStackPop(typeStack);
            int type2 = intStackPop(typeStack);
            //printf("type1: %i, type2: %i\n", type1, type2);

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
                
                assemble("DEFVAR",name,"","",instr);                        /// louda code
                char *theInt = malloc(10);

                symtableItem *val1 = symtableItemGet(actualFunc->key,value1); ///
                if (val1){
                    char *varrr = malloc(11+strlen(value1));                ///
                    if(!sprintf(theInt,"%d",val1->i))
                        return -INTERNAL_ERROR;
                    strcpy(varrr,value1);
                    varrr = strcat(varrr,"$");
                    varrr = strcat(varrr,theInt);
                    value1 = malloc(11+strlen(value1));
                    strcpy(value1,varrr);
                }

                symtableItem *val2 = symtableItemGet(actualFunc->key,value2); ///
                if (val2){                                                  ///
                    char *varrr = malloc(11+strlen(value2));
                    if(!sprintf(theInt,"%d",val2->i))
                        return -INTERNAL_ERROR;
                    strcpy(varrr,value2);
                    varrr = strcat(varrr,"$");
                    varrr = strcat(varrr,theInt);
                    value2 = malloc(11+strlen(value2));
                    strcpy(value2,varrr);
                }                        /// end of louda code
                
                //              typy vsetkych operandov vo switchi  su var
                switch(i) {
                    case 0:
                        assemble("ADD", name, value2, value1, instr);
                        break;
                    case 1:
                        assemble("SUB", name, value2, value1, instr);
                        break;
                    case 2:
                        assemble("MUL", name, value2, value1, instr);
                        break;
                    case 3:
                        if(type1 == INTEGER){
                            assemble("DIV", name, value2, value1, instr);
                        }
                        if(type1 == FLOAT) {
                            assemble("IDIV", name, value2, value1, instr);
                        }
                        break;
                    case 4:
                        assemble("LT", name, value2, value1, instr);
                    case 5:
                        assemble("LT", name, value2, value1, instr);
                        assemble("PUSHS", name, "", "", instr);
                        assemble("EQ", name, value2, value1, instr);
                        assemble("PUSHS", name, "", "", instr);
                        assemble("ORS", name, "", "", instr);
                    case 6:
                        assemble("GT", name, value2, value1, instr);
                    case 7:
                        assemble("GT", name, value2, value1, instr);
                        assemble("PUSHS", name, "", "", instr);
                        assemble("EQ", name, value2, value1, instr);
                        assemble("PUSHS", name, "", "", instr);
                        assemble("ORS", name, "", "", instr);
                    case 8:
                        assemble("EQ", name, value2, value1, instr);
                    case 9:
                        assemble("EQ", name, value2, value1, instr);
                        assemble("NOT", name, "", "", instr);
                    case 17:
                        assemble("AND", name, value2, value1, instr);
                    case 18:
                        assemble("OR", name, value2, value1, instr);
                }
                return i;
            }
            else
                return DIFFERENT_TYPES;
            return i;
        }
        
        if(i == 10) {   // E -> ID
            symtableItem *item = symtableItemGet(actualFunc->key, teken->value);
            int tmp = item->type;
            //printf("ID DATATYPE PUSED ON STACK: %i   \n", tmp);
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;
            //printf("po pushi\n");

            char *varrr = malloc(11+strlen(item->key)); /// louda code
            char *theInt = malloc(10);                  ///

            if(!sprintf(theInt,"%d",item->i))
                return -INTERNAL_ERROR;
            strcpy(varrr,item->key);
            varrr = strcat(varrr,"$");
            varrr = strcat(varrr,theInt);               /// louda code end
                
            
            assemble("DEFVAR", name,"","", instr);
            //               var   var
            assemble("MOVE", name, varrr, "", instr);
            return i;
        }
        if(i == 11) {   // E -> (E)
            //printf("ele1: %i\n", ele1->paType);
            int tmp = intStackPop(typeStack);
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;
            
            assemble("DEFVAR", name, "", "", instr);
            //               var   var 
            assemble("MOVE", name, value2, "", instr);

            return i;
        } 
        if(i == 12) {   // E -> i(int)
            *lastFoundType = INTEGER;
            int tmp = INTEGER;
            intStackPush(typeStack, tmp);
            //printf("\n\n %i PUSHED ON STACK\n", *typeStack->iStack[typeStack->top]);
             
            assemble("DEFVAR", name, "", "", instr);
            //               var   int
            assemble("MOVE", name, teken->value, "", instr);

            return i;
        }
        if(i == 13) {   // E -> i(float)
            *lastFoundType = FLOAT;
            int tmp = FLOAT; 
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", name, "", "", instr);
            //               var,  float
            assemble("MOVE", name, teken->value, "", instr);
            return i;
        }
        if(i == 14) {   // E -> i(string)
            *lastFoundType = STRING;
            int tmp = STRING; 
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", name, "", "", instr);
            //               var   string
            assemble("MOVE", name, teken->value, "", instr);

            return i;
        }
        if(i == 16) {   //  E -> !E
            int tmp = intStackPop(typeStack);
            *lastFoundType = tmp;
            intStackPush(typeStack, tmp);
        }
        if(i == 19) {   // E -> i(bool)
            *lastFoundType = BOOL;
            int tmp = BOOL;
            intStackPush(typeStack, tmp);
            
            assemble("DEFVAR", name, "", "", instr);
            //               var,  bool
            assemble("MOVE", name, teken->value, "", instr);

            return i;
        }

    }
    return -2;
}

int sFindRule(s_t *mainStack, s_t *tmpStack, sElemType *tmpTerminal, is_t *typeStack, int * lastFoundType, char *name) {
    int i = 0;
    int usedRule;
    int ruleOnStack[3] = {0, 0, 0};
    
    char *values[2] = {NULL, NULL};
    char *override = NULL;
    
    while ((mainStack->stack[mainStack->top])->paType != OP_LESS) {
        if(mainStack->stack[mainStack->top]->paToken->value != NULL)
            //printf("SFINDRULE VALUE ON TOP OF STACK : %s\n", mainStack->stack[mainStack->top]->paToken->value);
        sGetTopPointer(mainStack, tmpTerminal);
        if (!sPushPointer(tmpStack, tmpTerminal))
            return -1;
        
        if(i == 0) {
            if(mainStack->stack[mainStack->top]->paType == OP_EXPRESSION)
                values[0] = mainStack->stack[mainStack->top]->name;
            else {
                values[0] = mainStack->stack[mainStack->top]->paToken->value;
            }
        }
        if(i == 1 && mainStack->stack[mainStack->top]->paType == OP_EXPRESSION) {
            override = mainStack->stack[mainStack->top]->name;
        }
        if(i == 2) {
             if(mainStack->stack[mainStack->top]->paType == OP_EXPRESSION)
                values[1] = mainStack->stack[mainStack->top]->name;
            else
                values[1] = mainStack->stack[mainStack->top]->paToken->value;
        }
        sPopPointer(mainStack);
        i++;
    }
    
    if (tmpStack->top > 2)
        return -1;

    if(override != NULL) {
        values[0] = NULL;
        values[1] = override;
    }

    for (int i = tmpStack->top; i > -1; i--)    // save the content of stack until PA_LESS is read
        ruleOnStack[i] = (tmpStack->stack[i])->paType;
    
    if(tmpStack->top == 0)
        usedRule = generateRule(ruleOnStack, typeStack, lastFoundType, tmpStack->stack[0]->paToken, name, values[0], NULL);
    else
        usedRule = generateRule(ruleOnStack, typeStack, lastFoundType, tmpStack->stack[0]->paToken, name, values[0], values[1]);
 
    //printf("returning %i\n", usedRule);
    return usedRule;
}

int analyzePrecedence() {
    //printf("\nINA ZAVOLANA FUNKCIA\n");
 
    int action = 0;             // action determined by prectable
    int findRuleRet = 0;
    int lastFoundType = 0;      // type of most recently analyzed symbol or expression
    
    Token t;                    // currently analyzed token
    Token *paToken;             // helper Token
    sElemType *analyzedSymbol;  // currently analyzed Token and its paType in symbol structure
    
    sElemType tmpSymbol = {NULL, 0};  // helper symbol struct

    char *name = malloc(100 * sizeof(char));

    int precedentTable[25][25];
    pTableInit(precedentTable);

    // paToken struct init
    paToken = malloc(sizeof(Token));
    if(paToken == NULL) {        
        fprintf(stderr, "Malloc failed to allocate memory");
        return -908;
    }    
    paToken->value = NULL;
    
    // analyzedSymbol struct init
    analyzedSymbol = malloc(sizeof(sElemType));
    analyzedSymbol->paToken = paToken;
    sElemInit(analyzedSymbol);
    analyzedSymbol->name = malloc(ID_SIZE);
    
    // main symbol stack init
    s_t *mainStack;
    mainStack = malloc(sizeof(s_t));
    sInit(mainStack);
    tmpSymbol.paType = OP_DOLLAR;
    tmpSymbol.paToken = NULL;
    tmpSymbol.name = malloc(ID_SIZE);
    sPush(mainStack, &tmpSymbol);   // push $ onto stack
    
    // tmp symbol stack init
    s_t *tmpStack;
    tmpStack = malloc(sizeof(s_t));
    if(tmpStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return -908;
    }
    sInit(tmpStack);
   
    // integer stack for storage of types
    is_t *typeStack;
    typeStack = malloc(sizeof(is_t));
    if(typeStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return -908;
    }
    intStackInit(typeStack);       
    
    getToken(&t);   // get the first token of expression
    //if(t.value != NULL)
        //printf("\n\n\n%s\n\n\n", t.value);

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
                findRuleRet = sFindRule(mainStack, tmpStack, &tmpSymbol, typeStack, &lastFoundType, name);
                // rule does not exist -> syntactic error
                if(findRuleRet == -2) {
                    //printf("returning -2\n");
                    return -2;
                }

                // conflicting types of symbols -> semantic error
                if(findRuleRet == DIFFERENT_TYPES)
                {
                    //printf("returning -2\n");
                    return -5;
                }
                
                // pop the symbol on the top of the stack
                sPopPointer(mainStack);
                 
                tmpSymbol.paType = OP_EXPRESSION;
                tmpSymbol.paToken = (tmpStack->stack[tmpStack->top])->paToken;
                strcpy(tmpSymbol.name, name);
                
                //printf("tmpSymbol name: %s\n", tmpSymbol.name);
                // and replace it with Expression
                sPushPointer(mainStack, &tmpSymbol);
                while (!sIsEmpty(tmpStack)) {
		    sPopPointer(tmpStack);
                }
                break;
            case(PA_LESS):
                // copy symbol on stack until the first terminal is found onto tmpStack
                sCopyUntilTerminal(mainStack, tmpStack, &tmpSymbol);
                //push '<' onto the stack
                tmpSymbol.paToken = NULL;
		tmpSymbol.paType = OP_LESS;
                tmpSymbol.name = NULL;
		sPush(mainStack, &tmpSymbol);
                // copy the symbols from tmpStack into mainStack /reversed order/
                sCopyAll(tmpStack, mainStack, &tmpSymbol);
                // pushes the symbol just where it should be :3
                sPush(mainStack, analyzedSymbol);
                
                // get next symbol to analyze
                getToken(&t);
                //printf("t->type = %i", t.type);
                //printf("it->type: %i", t.type);
                break;
            case(PA_SHIFT):
                sPush(mainStack, analyzedSymbol);
                getToken(&t);
                //printf("\nHalo dpc: %i \n", t.type);
                break;
            case(PA_EMPTY):
                //printf("returning -2\n");
                // syntactic error
                return -2;
            }
        // updates the analyzed symbol if new Token was gotten
        sElemGetData(&t, analyzedSymbol);
    }
    
    //                var
    assemble("PUSHS", tmpSymbol.name, "", "", instr);
    ungetToken(&t);
   // printf("LAST FOUND: %i", lastFoundType);
    //printf("returning 0\n");
    return lastFoundType;
}
