//
// Created by louda on 11/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "ifj20.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include "dynamic_string.h"
#include "precanalysis.h"

int main(){

    int lexicalResult = 0; //Scanner return code
    Token token;

    // CHECK(prolog())
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

    // printf("--- Status code: %d\n", lexicalResult);
    return lexicalResult;
}
