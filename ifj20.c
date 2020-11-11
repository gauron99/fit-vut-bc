//
// Created by louda on 11/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "ifj20.h"
#include "error.h"
#include "scanner.h"

int rdBody(){
    return 0;
}

int main(){

    int lexicalResult = 0; //Scanner return code
    Token token;

    while(token.type != EOF_) {
        lexicalResult = getToken(&token);
        //printf(" \n Recognized token: %d, with value: %s with error code: %d \n", token.type, token.value.stringValue, result); 
        if(lexicalResult == 1) {
            return LEXICAL_ERROR;
        }
    }

    /*while (token->type != T_EOF)
    {
        CHECK(Get_Token(wrap.tkn))
    }*/

    printf("pohoda\n");
    return 0;
}