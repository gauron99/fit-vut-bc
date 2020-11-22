#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "precanalysis.h"
#include "scanner.h"

int getPaType(Token *token) {
    switch(token->type) {
        case IDENTIFIER:
            return OP_ID;
        case INTEGER:
        case FLOAT:
        case STRING:
	    return OP_DATATYPE;
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
        case EOL_:
        case COMMA:
        case SEMICOLON:
        case LEFT_CURLY_BRACKET:
            return OP_DOLLAR;
    }
    return OP_DOLLAR;       // oddelat
}

bool sInit (s_t* s) {
    s->size = STACK_SIZE;
    s->top = STACK_TOP;
    s->stack = malloc(STACK_SIZE * sizeof(sElemType *));
    if (s->stack == NULL)
        return false;
    return true;
}

bool sIsEmpty (s_t* s) {
    return ((s->top == STACK_TOP) ? true : false);
}

bool sIsFull (s_t* s) {
    return ((s->top == s->size-1) ? true : false);
}

void sGetTop (s_t* s, sElemType *data) {
    if(!sIsEmpty(s)) {
        //kopiruje hodnoty
        data->paType = (s->stack[s->top])->paType;
        data->paToken->type = (s->stack[s->top])->paToken->type;
        data->paToken->value.intValue = (s->stack[s->top])->paToken->value.intValue;
        data->paToken->value.floatValue = (s->stack[s->top])->paToken->value.floatValue;
        memcpy(data->paToken->value.stringValue, (s->stack[s->top])->paToken->value.stringValue, sizeof((s->stack[s->top])->paToken->value.stringValue));
        }
    else
        return;
}

void sGetTopPointer(s_t *s, sElemType *data) {
    data->paType = (s->stack[s->top])->paType;
    data->paToken = (s->stack[s->top])->paToken;
}

int sGetTopTerminal(s_t * s) {
    int tmp = s->top;
    while ((s->stack[tmp])->paType == OP_LESS || (s->stack[tmp])->paType == OP_EXPRESSION) {
        tmp--;
    }
    return (s->stack[tmp])->paType;
}

void sPop (s_t* s) {
    if (!sIsEmpty(s)) {
        free((s->stack[s->top])->paToken->value.stringValue);
        free((s->stack[s->top])->paToken);
        free(s->stack[s->top]);
        s->top--;
    }
}

void sPopPointer(s_t *s) {
    if (!sIsEmpty(s)) {
	free(s->stack[s->top]);
	s->stack[s->top] = NULL;
	s->top--;
    }
}

bool sPushPointer(s_t *s, sElemType *data) {
    if (!sIsFull(s)) {
        sElemType *newData;
        if ((newData = malloc(sizeof(sElemType))) == NULL)
            return false;
        newData->paType = data->paType;
        newData->paToken = data->paToken;
        s->top++;
        s->stack[s->top] = newData;
    }
    else {
        sElemType **helpPtr = NULL;
        s->size *= 2;
        helpPtr = realloc(s->stack, s->size*sizeof(sElemType *));
        if (helpPtr == NULL) {
            return false;
        }
        s->stack = helpPtr;
        if(!sPushPointer(s, data))
            return false;
    }
    return true;
}

bool sPush (s_t* s, sElemType *newData) {
    if (!sIsFull(s)) {
        sElemType *data = NULL;
        Token *paToken = NULL;
        
        if ((data = malloc(sizeof(sElemType))) == NULL)
            return false;
        if ((paToken = malloc(sizeof(Token))) == NULL) {
            free(data);
            return false;
        }
    
        if ((paToken->value.stringValue = malloc(50 * sizeof(char))) == NULL) {
            free(paToken);
            free(data);
            return false;
        }
        
        paToken->value.stringValue = malloc(50 * sizeof(char));
       
        data->paToken = paToken;
        
        data->paType = newData->paType;
        
        if (newData->paToken != NULL) {
            data->paToken->type = newData->paToken->type;
            data->paToken->value.intValue = newData->paToken->value.intValue;
            data->paToken->value.floatValue = newData->paToken->value.floatValue;
            memcpy(&data->paToken->value.stringValue, &newData->paToken->value.stringValue,sizeof(newData->paToken->value.stringValue));
        }

        s->top++;
        s->stack[s->top] = data; 
    }
    else {
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
    return true;
}

void sFree(s_t *s) {
    if (s != NULL) {
        for (int i = s->top; i > -1; i--) {
            free(s->stack[i]);
            free(s->stack);
            free(s);
        }
    }
}

void sElemInit(sElemType *data) {
    data->paToken->type = 0;
    data->paToken->value.intValue = 0;
    data->paToken->value.floatValue = 0.0;
    data->paToken->value.stringValue = NULL;
    data->paType = 0;
}

void sElemFree(sElemType *data, Token *paToken) {
    if (paToken != NULL) {
        if (paToken->value.stringValue != NULL) {
            //strFree(paToken->value.stringValue);
            free(paToken->value.stringValue);
        }
        free(paToken);
    }
    free(data);
}

int sFindFirstTerminal(s_t *s) {
    int tmp = s->top;
     
    while ((s->stack[tmp])->paType == OP_EXPRESSION || (s->stack[tmp])->paType == OP_LESS) {
        tmp--;
    }
    return (s->stack[tmp])->paType;
}

void fill_token(Token *token, int type, int integer, double floating, char *string) {
	if (type == INTEGER) token->value.intValue = integer;
	else if (type == FLOAT) token->value.floatValue = floating;
	//else if (type == STRING) //memcpy(token->value.stringValue, string, sizeof(string));
        //strcpy(token->value.stringValue, string);
}

bool sElemGetData(Token *token, sElemType *data) {
    data->paType = getPaType(token);
    //printf("%s\n", token->value.stringValue);
    if(data->paType == OP_DATATYPE) {
        if(token->type == STRING)
            fill_token(data->paToken, STRING, 0, 0, token->value.stringValue);
        else if(token->type == INTEGER)
            fill_token(data->paToken, INTEGER, token->value.intValue, 0, NULL);
        else if(token->type == FLOAT)
            fill_token(data->paToken, FLOAT, 0, token->value.floatValue, NULL);
    }
    //else if(data->paType == OP_ID)
    //DOROBIT PRE ID, NECH GETNE HODNOTU ZREJME
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

int generateRule(int *rule) {
        //printf("\nRule used: %i %i %i\n", rule[0], rule[1],rule[2]);
	if (cmp_rules(rule, rule1)) return 1;
	else if (cmp_rules(rule, rule2)) return 2;
	else if (cmp_rules(rule, rule3)) return 3;
	else if (cmp_rules(rule, rule4)) return 4;
	else if (cmp_rules(rule, rule5)) return 5;
	else if (cmp_rules(rule, rule6)) return 6;
	else if (cmp_rules(rule, rule7)) return 7;
	else if (cmp_rules(rule, rule8)) return 8;
	else if (cmp_rules(rule, rule9)) return 9;
	else if (cmp_rules(rule, rule10)) return 10;
	else if (cmp_rules(rule, rule11)) return 11;
	else if (cmp_rules(rule, rule12)) return 12;
	else if (cmp_rules(rule, rule13)) return 13;
	else return -1;
}

int sFindRule(s_t *mainStack, s_t *tmpStack, sElemType *tmpTerminal) {
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
    int usedRule = generateRule(ruleOnStack);
    return usedRule;
}

int analyzePrecedence() {
    //printf("zavolana fun");
    Token *paToken;
    sElemType *mainTerminal;

    paToken = malloc(sizeof(Token));
    paToken->value.stringValue = malloc(sizeof(char *));
    paToken->value.stringValue = NULL;
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
    
    int prec_tab[15][15];
    pTableInit(prec_tab);

    int i = 0;
    int j = 0;
    Token t;
    getToken(&t);
    //printf("%i\n", t.type);
    int action = 0;
    sElemGetData(&t, mainTerminal);
    
    while(!(mainTerminal->paType == OP_DOLLAR && sGetTopTerminal(mainStack) == OP_DOLLAR)) {
            //printf("\ntop stack: %i, mainterm: %i\n", sGetTopTerminal(mainStack), mainTerminal->paType);
        i++;
        action = prec_tab[sGetTopTerminal(mainStack)][mainTerminal->paType];
        //printf("%i\n", action);
        switch(action) {
            case(PA_GREATER):
                if(sFindRule(mainStack, tmpStack, &tmpTerminal) == -1)
                    return 2;
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
        sElemGetData(&t, mainTerminal);
    }
    ungetToken(&t);
    //printf("RETURNING 0");

    return 0;
}
