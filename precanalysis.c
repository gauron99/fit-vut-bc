#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "parser.h"
#include "scanner.h"

extern symtableGlobalItem *actualFunc;

bool intStackInit(is_t *s) {
    s->top = STACK_TOP;
    s->size = STACK_SIZE;
    s->iStack = malloc(STACK_SIZE * sizeof(int *));
    if (s->iStack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
    return true;
}

void intStackFree(is_t *s) {
    if (s != NULL) {
        for (int i = s->top; i > STACK_TOP; i--) {
            free(s->iStack[i]);
        }
        free(s->iStack);
        free(s);
    }
}

bool intStackIsFull(is_t *s) {
    if(s->top == s->size - 1)
        return true;
    return false;

}

bool intStackIsEmpty(is_t *s) {
    if(s->top == STACK_TOP)
        return true;
    return false;
}

bool intStackPush(is_t *s, int data) {
    if (intStackIsFull(s)) {
        int **helpPtr = NULL;
        s->size = s->size * 2;
        helpPtr = realloc(s->iStack, s->size*sizeof(int *));
        if (helpPtr == NULL) {
                return false;
        }
        s->iStack = helpPtr;
        if(!intStackPush(s, data))
                return false;
    }
    else {
        int *tmp = malloc(sizeof(int));
        *tmp = data;
        s->top++;
        s->iStack[s->top] = tmp;
    }
    return true;
}

int intStackPop(is_t *s) {
    if(!intStackIsEmpty(s)) {
        int ret = *s->iStack[s->top];
        free(s->iStack[s->top]);
        s->top--;
        return ret;
    }
    else
        return -1;   
}

int getPaType(Token *token) {
    switch(token->type) {
        case KEYWORD:
            return OP_BOOL;

        case IDENTIFIER: {
            /*printf("HALOHOHOHO");
            if(token->value != NULL) {
                printf("\nPATYPEVALUE: %s\n", token->value);
            }*/
            Token t1,t2;
            getToken(&t1);
            if(t1.type == LEFT_ROUND_BRACKET) {
                getToken(&t2);
                if(t2.type == RIGHT_ROUND_BRACKET) {
                    return OP_FUN;
                }
                else {
                    ungetToken(&t1);
                    ungetToken(&t2);
                    return OP_FUN;
                }
            }
            ungetToken(&t1);
            //printf("RETURNING OP_ID in getPaType");
            return OP_ID;
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
        case RIGHT_ROUND_BRACKET:
            return OP_RBRAC;
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
    return OP_DOLLAR;       // oddelat
}

bool sInit (s_t* s) {
    s->top = STACK_TOP;
    s->size = STACK_SIZE;
    s->stack = malloc(STACK_SIZE * sizeof(sElemType *));
    if (s->stack == NULL) {
        fprintf(stderr, "Malloc failed to allocate memory");
        return false;
    }
    return true;
}

bool sIsEmpty (s_t* s) {
    if(s->top == STACK_TOP)
        return true;
    return false;
}

bool sIsFull (s_t* s) {
    if(s->top == s->size - 1)
        return true;
    return false;
}

void sGetTop (s_t* s, sElemType *data) {
    if(sIsEmpty(s)) {
        // error
        return;
    }
    else {
        data->paType = (s->stack[s->top])->paType;
        strcpy((s->stack[s->top])->paToken->value, data->paToken->value);
        data->paToken->type = (s->stack[s->top])->paToken->type;
        return;
    }
}

void sGetTopPointer(s_t *s, sElemType *data) {
    data->paType = (s->stack[s->top])->paType;
    data->paToken = (s->stack[s->top])->paToken;
    data->paToken->value = (s->stack[s->top])->paToken->value;
    //printf("\n-------------sgettoppointer value : %s", data->paToken->value);
    return;
}

int sFindFirstTerminal(s_t * s) {
    int tmp = s->top;

    while ((s->stack[tmp])->paType == OP_LESS || (s->stack[tmp])->paType == OP_EXPRESSION) {
        tmp--;
    }

    return (s->stack[tmp])->paType;
}

void sPop (s_t* s) {
    if (sIsEmpty(s)) {
        return;
    }
    else {
        //free((s->stack[s->top])->paToken->value.stringValue);
        free((s->stack[s->top])->paToken);
        free(s->stack[s->top]);
        s->top--;
    }
}

void sPopPointer(s_t *s) {
    if (sIsEmpty(s)) {
        return;
    }
    else {
	free(s->stack[s->top]);
	s->stack[s->top] = NULL;
	s->top--;
    }
}

bool sPushPointer(s_t *s, sElemType *data) {
    if (sIsFull(s)) {
         sElemType **helpPtr = NULL;
        s->size += 10;
        helpPtr = realloc(s->stack, s->size*sizeof(sElemType *));
        if (helpPtr == NULL) {
            return false;
        }
        s->stack = helpPtr;
        if(!sPushPointer(s, data))
            return false;
    }
    else {
        /*if(data != NULL) {
            //printf("data != nULL");
            if(data->paToken->value != NULL)
                //printf("VALUE: %s\n", data->paToken->value);
        }*/
        sElemType *newData;
        if ((newData = malloc(sizeof(sElemType))) == NULL)
            return false;
        newData->paType = data->paType;
        newData->paToken = data->paToken;
        s->top++;
        s->stack[s->top] = newData;
    }
    return true;
}

bool sPush (s_t* s, sElemType *newData) {
    if (sIsFull(s)) {
        sElemType **helpPtr = NULL;
        s->size = s->size + 10;
        helpPtr = realloc(s->stack, s->size*sizeof(sElemType *));
        if (helpPtr == NULL) {
                return false;
        }
        s->stack = helpPtr;
        if(!sPush(s, newData))
                return false;
    }
    else {
        sElemType *data = NULL;
        Token *paToken = NULL;
        //char *val = NULL; 
        
        if(newData->paToken != NULL) {
            if(newData->paToken->value != NULL)
                //printf("\nVALUE: %s", newData->paToken->value);
        }
        
        if ((data = malloc(sizeof(sElemType))) == NULL)
            return false;
        if ((paToken = malloc(sizeof(Token))) == NULL) {
            free(data);
            return false;
        }
         
        data->paToken = paToken;
        
        data->paType = newData->paType;
        
        //data->paToken->value = val;


        if (newData->paToken != NULL) {
            data->paToken->type = newData->paToken->type;
            //memcpy(data->paToken->value, newData->paToken->value, strlen(newData->paToken->value)+1);
            //strcpy(data->paToken->value, newData->paToken->value);
        }

        s->top++;
        s->stack[s->top] = data; 
    }
    return true;
}

void sFree(s_t *s) {
    if (s != NULL) {
        for (int i = s->top; i > STACK_TOP; i--) {
            free(s->stack[i]);
        }
        free(s->stack);
        free(s);
    }
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
    if(data->paToken->value != NULL) {
        strcpy(token->value,data->paToken->value);
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

bool cmp_rules(int *stackay1, int *stackay2) {
    for (int i = 0; i < 3; i++) {
    	if (stackay1[i] != stackay2[i])
	    return false;
    }
    return true;
}

int generateRule(int *rule, is_t *typeStack, int *lastFoundType, Token *teken) {
    //printf("\nRule found: %i %i %i\n", rule[0], rule[1],rule[2]);
    sElemType *tmp = malloc(sizeof(sElemType));

    int i = 0;
    int found = 0;
    
    while(i < 20) {
        if(rule[0] == rules[i][0] && rule[1] == rules[i][1] && rule[2] == rules[i][2]) {
            //return i;
            found = 1;
            break;
        }
        i++;
    }
    
    if(found == 1) {
        //printf("CHECK 2, i = %i\n", i);
        if(i < 10 || i == 17 || i == 18) {

            int type1 = intStackPop(typeStack);
            int type2 = intStackPop(typeStack);

            if(type1 == type2) {
                if(i < 4 || i == 17 || i == 18) {
                    intStackPush(typeStack, type1);
                    *lastFoundType = type1;
                }
                else {
                    type2 = BOOL;
                    intStackPush(typeStack, type2);
                    *lastFoundType = BOOL;
                }
                return i;
            }
            else
                return DIFFERENT_TYPES;
            return i;
        }
        if(i == 10) {
            //printf("SOM TUUUUUUU, teken val = %s\n", teken->value);
            symtableItem *item = symtableItemGet(actualFunc->key, teken->value);
            int tmp = *item->key;
            //printf("ID DATATYPE PUSED ON STACK: %i\n", tmp);
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;
            //printf("po pushi\n");
            return i;
        }
        if(i == 11) {
            //printf("ele1: %i\n", ele1->paType);
            int tmp = intStackPop(typeStack);
            intStackPush(typeStack, tmp);
            *lastFoundType = tmp;
            return i;
        } 
        if(i == 12) {
            *lastFoundType = INTEGER;
            int tmp = INTEGER;
            intStackPush(typeStack, tmp);
            //printf("\n\n %i PUSHED ON STACK\n", *typeStack->iStack[typeStack->top]);
            return i;
        }
        if(i == 13) {
            *lastFoundType = FLOAT;
            int tmp = FLOAT; 
            intStackPush(typeStack, tmp);
            return i;
        }
        if(i == 14) {
            *lastFoundType = STRING;
            int tmp = STRING; 
            intStackPush(typeStack, tmp);
            return i;
        }
        if(i == 16) {
            int tmp = intStackPop(typeStack);
            *lastFoundType = tmp;
            intStackPush(typeStack, tmp);
        }
    }
    return -2;
}

int sFindRule(s_t *mainStack, s_t *tmpStack, sElemType *tmpTerminal, is_t *typeStack, int * lastFoundType) {
    while ((mainStack->stack[mainStack->top])->paType != OP_LESS) {
        sGetTopPointer(mainStack, tmpTerminal);
        if (!sPushPointer(tmpStack, tmpTerminal))
            return -1;
        sPopPointer(mainStack);
    }
    if (tmpStack->top > 2)
        return -1;
    int ruleOnStack[3] = {0, 0, 0};
    for (int i = tmpStack->top; i > -1; i--)
        ruleOnStack[i] = (tmpStack->stack[i])->paType;
    int usedRule = generateRule(ruleOnStack, typeStack, lastFoundType, tmpStack->stack[0]->paToken);
    
    //printf("returning %i\n", usedRule);
    return usedRule;
}

int analyzePrecedence() {
    //printf("\nINA ZAVOLANA FUNKCIA\n");
    Token *paToken;
    sElemType *mainTerminal;
   
    int lastFoundType = 0;

    paToken = malloc(sizeof(Token));
    paToken->value = malloc(sizeof(char *));
    paToken->value = NULL;
    
    mainTerminal = malloc(sizeof(sElemType));

    mainTerminal->paToken = paToken;
    sElemInit(mainTerminal);

    sElemType tmpTerminal = {NULL, 0};

    s_t *mainStack;
    mainStack = malloc(sizeof(s_t));
    sInit(mainStack);
    tmpTerminal.paType = OP_DOLLAR;
    sPush(mainStack, &tmpTerminal);

    s_t *tmpStack;
    tmpStack = malloc(sizeof(s_t));
    sInit(tmpStack);

    s_t *typeStack;
    typeStack = malloc(sizeof(s_t));
    sInit(typeStack);
    
    is_t *intStack;
    intStack = malloc(sizeof(is_t));
    intStackInit(intStack);
       
    int prec_tab[25][25];
    pTableInit(prec_tab);
    int ret = 0;
    int i = 0;
    Token t;
    getToken(&t);
    int action = 0;
    sElemGetData(&t, mainTerminal);
    while(!(mainTerminal->paType == OP_DOLLAR && sFindFirstTerminal(mainStack) == OP_DOLLAR)) {
        //printf("\ntop stack: %i, mainterm: %i\n", sFindFirstTerminal(mainStack), mainTerminal->paType);
        i++;
        action = prec_tab[sFindFirstTerminal(mainStack)][mainTerminal->paType];
        //printf("%i\n", action);
        switch(action) {
            case(PA_GREATER):
                ret = sFindRule(mainStack, tmpStack, &tmpTerminal, intStack, &lastFoundType);
                if(ret == -2) {
                    //printf("returning -2\n");
                    return -2;
                }
                if(ret == DIFFERENT_TYPES)
                {
                    //printf("returning -2\n");
                    return -5;
                }
                
                sPopPointer(mainStack);
                 
                tmpTerminal.paType = OP_EXPRESSION;
                tmpTerminal.paToken = (tmpStack->stack[tmpStack->top])->paToken;

                sPushPointer(mainStack, &tmpTerminal);
                while (!sIsEmpty(tmpStack)) {
		    sPopPointer(tmpStack);
                }
                break;
            case(PA_LESS):
                sCopyUntilTerminal(mainStack, tmpStack, &tmpTerminal);
                tmpTerminal.paToken = NULL;
		tmpTerminal.paType = OP_LESS;
		sPush(mainStack, &tmpTerminal);
                sCopyAll(tmpStack, mainStack, &tmpTerminal);
                sPush(mainStack, mainTerminal);
                getToken(&t);
                //printf("t->type = %i", t.type);
                //printf("it->type: %i", t.type);
                break;
            case(PA_SHIFT):
                sPush(mainStack, mainTerminal);
                getToken(&t);
                //printf("\nHalo dpc: %i \n", t.type);
                break;
            case(PA_EMPTY):
                //printf("returning -2\n");
                return -2;
            }
        sElemGetData(&t, mainTerminal);
    }
    
    ungetToken(&t);
    //printf("LAST FOUND: %i", lastFoundType);
    //printf("returning 0\n");
    return 0;
}
