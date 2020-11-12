//
// Created by louda on 11/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "ifj20.h"
#include "error.h"
#include "scanner.h"
#include "custom_string.h"
#include "precanalysis.h"
#include "parser.h"

int rdBody(){
    return 0;
}

int main(){

    int lexicalResult = 0; //Scanner return code
    Token token;

    while(token.type != EOF_) {
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
    }

    /*while (token->type != T_EOF)
    {
        CHECK(Get_Token(wrap.tkn))
    }*/

    printf("--- Status code: %d\n", lexicalResult);
    return lexicalResult;
}
