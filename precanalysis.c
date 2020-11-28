#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "scanner.h"

int getPaType(Token *token, s_t *typeStack) {
    sElemType typeElement;
    switch(token->type) {
        case IDENTIFIER: {
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
            return OP_ID;
        }
        case INTEGER:{ 
            typeElement.paType = INTEGER;
            sPush(typeStack, &typeElement);
            return OP_INTEGER;
        }
        case FLOAT: {
            typeElement.paType = FLOAT;
            sPush(typeStack, &typeElement);
            return OP_FLOAT;
        }

        case STRING: {
            typeElement.paType = STRING;
            sPush(typeStack, &typeElement);
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
        
        if ((data = malloc(sizeof(sElemType))) == NULL)
            return false;
        if ((paToken = malloc(sizeof(Token))) == NULL) {
            free(data);
            return false;
        }
    
        if ((paToken->value = malloc(50 * sizeof(char))) == NULL) {
            free(paToken);
            free(data);
            return false;
        }
        
        data->paToken = paToken;
        
        data->paType = newData->paType;
        
        if (newData->paToken != NULL) {
            data->paToken->type = newData->paToken->type;
            strcpy(data->paToken->value, newData->paToken->value);
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
            free(s->stack);
            free(s);
        }
    }
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

bool sElemGetData(Token *token, sElemType *data, s_t *typeStack) {
    data->paType = getPaType(token, typeStack);
    data->paToken->type = token->type;
    strcpy(token->value,data->paToken->value);
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

int generateRule(int *rule, s_t *typeStack, int *lastFoundType) {
    //printf("\nRule used: %i %i %i\n", rule[0], rule[1],rule[2]);
    sElemType ele1, ele2;
    int i = 0;
    int found = 0;
    for(; i < 20; i++) {
        if(rule[0] == rules[i][0] && rule[1] == rules[i][1] && rule[2] == rules[i][2]) {
            found = 1;
            break;
        }
    }

    if(found == 1) {
        if(i < 10) {
            sGetTop(typeStack, &ele1);
            sPopPointer(typeStack);
            sGetTop(typeStack, &ele2);
            sPopPointer(typeStack);
            if(ele1.paType == ele2.paType) {
                if(found < 4)
                    *lastFoundType = ele1.paType;
                else
                    *lastFoundType = BOOL;
                return i;
            }
            else
                return DIFFERENT_TYPES;
        }
        return i;
    }   // domysliet pre ID
    else if(found == 11) {
        sGetTop(typeStack, &ele1);
        sPopPointer(typeStack);
        *lastFoundType = ele1.paType;
        return i;
    } else if(found == 12) {
        *lastFoundType = INTEGER;
        return i;
    } else if(found == 13) {
        *lastFoundType = FLOAT;
        return i;
    } else if(found == 14) {
        *lastFoundType = STRING;
        return i;
    }

    return -1;
}

int sFindRule(s_t *mainStack, s_t *tmpStack, sElemType *tmpTerminal, s_t *typeStack, int * lastFoundType) {
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
    int usedRule = generateRule(ruleOnStack, typeStack, lastFoundType);
    
    return usedRule;
}

int analyzePrecedence() {
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
    
    int prec_tab[25][25];
    pTableInit(prec_tab);
    int ret = 0;
    int i = 0;
    Token t;
    getToken(&t);
    int action = 0;
    sElemGetData(&t, mainTerminal, typeStack);
    while(!(mainTerminal->paType == OP_DOLLAR && sFindFirstTerminal(mainStack) == OP_DOLLAR)) {
            //printf("\ntop stack: %i, mainterm: %i\n", sFindFirstTerminal(mainStack), mainTerminal->paType);
        i++;
        action = prec_tab[sFindFirstTerminal(mainStack)][mainTerminal->paType];
        //printf("%i\n", action);
        switch(action) {
            case(PA_GREATER):
                ret = sFindRule(mainStack, tmpStack, &tmpTerminal, typeStack, &lastFoundType);
                if(ret == -1)
                    return 2;
                if(ret == DIFFERENT_TYPES)
                    return DIFFERENT_TYPES;
                
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
                //printf("it->type: %i", t.type);
                break;
            case(PA_SHIFT):
                sPush(mainStack, mainTerminal);
                getToken(&t);
            case(PA_EMPTY):
                //printf("RETURNING 2");
                return 2;
        }
        sElemGetData(&t, mainTerminal, typeStack);
    }
    ungetToken(&t);
    PUSHS(lastFoundType);
    return 0;
}
