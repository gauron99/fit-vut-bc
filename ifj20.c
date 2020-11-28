//
// Created by louda on 11/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "ifj20.h"
#include "error.h"
#include "scanner.h"
#include "custom_string.h"
#include "parser.h"
#include "symtable.h"

symtableGI symGlobal;
extern Token* tkns;
extern int dewit;

int allocUnget(){
    if (!(tkns = malloc(sizeof(Token))))
        return INTERNAL_ERROR;
    return EC_GOOD;
}

int main(){

    //create global symtable
    symtableCreateGlobal(&symGlobal); //works
    CHECK(allocUnget())
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//



    int lexicalResult = 0; //Scanner return code
    Token token;

    /*while(token.type != EOF_) {
        lexicalResult = getToken(&token);
        if(lexicalResult == 1) {
            break;
        }
        if(token.type == INTEGER) {
            printf(" \n Recognized token: %d, with value: %d with error code: %d \n", token.type, token.value.intValue, lexicalResult);
        } else if(token.type == FLOAT) {
            printf(" \n Recognized token: %d, with value: %f with error code: %d \n", token.type, token.value.floatValue, lexicalResult);
        } else {
            printf(" \n Recognized token: %d, with value: %s with error code: %d \n", token.type, token.value.stringValue, lexicalResult);
        }
    }*/
    CHECK(prolog())

    //printf("--- Status code: %d\n", lexicalResult);
    free(tkns);
    return lexicalResult;
    return 0;
}
