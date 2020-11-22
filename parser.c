//
// Created by louda on 11/11/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ifj20.h"
#include "scanner.h"
#include "parser.h"
#include "precanalysis.h"

Token token;

int precedence(){
    while (1){
        CHECK(getToken(&token));
        if (TTYPE==EOL_||TTYPE==SEMICOLON||TTYPE==COMMA||TTYPE==LEFT_CURLY_BRACKET) {
            ungetToken(&token);
            break;
        }
        else
            continue;
    }
    return EC_GOOD;
}

int isType(char* string){
    if (!strcmp(string,"int") || !strcmp(string,"float64") || !strcmp(string,"string") || !strcmp(string,"bool"))
        return 1;
    return 0;
}

int prolog(){
    CHECK(getToken(&token));
    if (TTYPE == EOL_)
    CHECK(prolog())
    else if (TTYPE == KEYWORD){
        CHECK_R(!strcmp(TSTR,"package"),EC_SYN)
        CHECK(getToken(&token));
        CHECK_R(TTYPE == IDENTIFIER && !strcmp(TSTR,"main"),EC_SYN)
        CHECK(rdBody())
    }
    else
        return EC_SYN;
    return EC_GOOD;
}

int rdBody(){
    CHECK(getToken(&token));
    if (TTYPE==EOF_)
        return EC_GOOD;
    else if (TTYPE == EOL_)
    CHECK(rdBody())
    else {
        CHECK_R(TTYPE==KEYWORD && !strcmp(TSTR,"func"),EC_SYN) // snad nebude padat se spatnym typem tokenu

        CHECK(rdDef())
        CHECK(rdBody())
    }
    return EC_GOOD;
}

int rdDef(){
    CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    CHECK(getToken(&token));

    CHECK_R(TTYPE==LEFT_ROUND_BRACKET,EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE!=RIGHT_ROUND_BRACKET)
    CHECK(rdParams())

    CHECK(getToken(&token));

    if (TTYPE!=LEFT_CURLY_BRACKET)
    CHECK(rdReturns())

    CHECK(getToken(&token));

    CHECK_R(TTYPE==EOL_,EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE!=RIGHT_CURLY_BRACKET)
    CHECK(rdComm())

    return EC_GOOD;
}

int rdParams(){
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE == COMMA) {
        CHECK(getToken(&token));
        CHECK(rdParamsN())
    }
    else
    CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdParamsN(){
    if (TTYPE==EOL_)
        CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE==COMMA) {
        CHECK(getToken(&token));
        CHECK(rdParamsN())
    }

    CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdReturns(){
    CHECK_R(TTYPE==LEFT_ROUND_BRACKET,EC_SYN)
    CHECK(getToken(&token));
    if (TTYPE!=RIGHT_ROUND_BRACKET) {
        CHECK(rdReturnsNamed())
        CHECK(getToken(&token));
    }
    else
        CHECK(getToken(&token));


    CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdReturnsNamed(){
    if (TTYPE==IDENTIFIER){
        CHECK(getToken(&token));
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)
        CHECK(getToken(&token));
        if (TTYPE==COMMA)
        CHECK(rdReturnsNamedN())
    }
    else {
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)
        CHECK(getToken(&token));
        if (TTYPE==COMMA)
        CHECK(rdReturnsN())
    }


    return EC_GOOD;
}

int rdReturnsNamedN(){
    CHECK(getToken(&token));
    if (TTYPE==EOL_)
        CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE==COMMA)
    CHECK(rdReturnsNamedN())

    return EC_GOOD;
}

int rdReturnsN(){
    CHECK(getToken(&token));
    if (TTYPE==EOL_)
        CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(getToken(&token));

    if (TTYPE==COMMA)
    CHECK(rdReturnsNamedN())

    return EC_GOOD;
}

int rdComm(){

    if (TTYPE==EOL_){
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"if"))){
        CHECK(analyzePrecedence());
        CHECK(getToken(&token));

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())

        CHECK(getToken(&token));

        if (TTYPE!=EOL_)
        CHECK(rdElseOrNot())
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"for"))){
        CHECK(getToken(&token));
        if (TTYPE!=SEMICOLON){
            CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

            CHECK(getToken(&token));

            if (TTYPE!=DEFINITION){
                CHECK_R(TTYPE==COMMA,EC_SYN)
                CHECK(getToken(&token));
                CHECK(rdIdsekv())
            }
            CHECK_R(TTYPE==DEFINITION,EC_SYN)
            CHECK(getToken(&token));
            CHECK(rdExprsOrCall())
        }
        CHECK_R(TTYPE==SEMICOLON,EC_SYN)
        CHECK(analyzePrecedence());

        CHECK(getToken(&token));
        CHECK_R(TTYPE==SEMICOLON,EC_SYN)

        CHECK(getToken(&token));
        if (TTYPE!=LEFT_CURLY_BRACKET) {
            CHECK(rdIdsekv())
            CHECK_R(TTYPE==ASSIGNMENT,EC_SYN)
            CHECK(getToken(&token));
            CHECK(rdExprsOrCall())
        }

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
        CHECK_R(TTYPE==RIGHT_CURLY_BRACKET,EC_SYN)
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !strcmp(TSTR,"return")){
        CHECK(getToken(&token));
        if (TTYPE!=EOL_){
            CHECK(rdExprSekv())
            CHECK_R(TTYPE==EOL_,EC_SYN)
        }
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==IDENTIFIER){
        CHECK(getToken(&token));
        if (TTYPE==COMMA || TTYPE == ASSIGNMENT || TTYPE == DEFINITION){
            if (TTYPE==COMMA){
                CHECK(getToken(&token));
                CHECK(rdIdsekv())
                CHECK_R(TTYPE==ASSIGNMENT||TTYPE==DEFINITION,EC_SYN)
            }
            CHECK(getToken(&token));
            CHECK(rdExprsOrCall())
        }
        else if (TTYPE==LEFT_ROUND_BRACKET){
            CHECK(getToken(&token));
            if (TTYPE!=RIGHT_ROUND_BRACKET)
            CHECK(rdInParams())
            CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
            CHECK(getToken(&token));
        }
        else if (TTYPE==PLUS_EQ||TTYPE==MINUS_EQ||TTYPE==MUL_EQ||TTYPE==DIV_EQ){
            CHECK(analyzePrecedence());
            CHECK(getToken(&token));
        }
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else
    CHECK_R(TTYPE==RIGHT_CURLY_BRACKET,EC_SYN)


    if (TTYPE!=RIGHT_CURLY_BRACKET)
    CHECK(rdComm())

    return EC_GOOD;
}

int rdElseOrNot(){
    CHECK_R(TTYPE==KEYWORD && !strcmp(TSTR,"else"),EC_SYN)
    CHECK(getToken(&token));

    if (TTYPE==LEFT_CURLY_BRACKET){
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));
        if(TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())
    }
    else {
        CHECK_R(TTYPE == KEYWORD && !strcmp(TSTR, "if"), EC_SYN)
        CHECK(analyzePrecedence());
        CHECK(getToken(&token));

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())

        CHECK(getToken(&token));

        if (TTYPE!=EOL_)
        CHECK(rdElseOrNot())
    }

    return EC_GOOD;
}

int rdExprsOrCall(){
    if (TTYPE==IDENTIFIER){
        Token tmpToken;
        tmpToken = token;
        CHECK(getToken(&token));
        if (TTYPE!=LEFT_ROUND_BRACKET){
            ungetToken(&token);
            ungetToken(&tmpToken);
            CHECK(analyzePrecedence());
            CHECK(getToken(&token));
            if (TTYPE==COMMA)
            CHECK(rdExprSekv())
        }
        else {
            CHECK(getToken(&token));
            if (TTYPE!=RIGHT_ROUND_BRACKET) {
                CHECK(rdInParams())
                CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
            }
            CHECK(getToken(&token));
        }
    }
    else {
        ungetToken(&token);
        CHECK(analyzePrecedence());
        CHECK(getToken(&token));
        if (TTYPE==COMMA)
            CHECK(rdExprSekv())
    }

    return EC_GOOD;
}

int rdIdsekv(){
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    CHECK(getToken(&token));
    if (TTYPE==COMMA) {
        CHECK(getToken(&token));
        CHECK(rdIdsekv())
    }

    return EC_GOOD;
}

int rdExprSekv(){
    CHECK(analyzePrecedence());
    CHECK(getToken(&token));
    if(TTYPE==COMMA)
        CHECK(rdExprSekv());

    return EC_GOOD;
}

int rdInParams(){
    CHECK_R(TTYPE==IDENTIFIER || TTYPE==STRING || TTYPE==FLOAT || TTYPE==INTEGER,EC_SYN)
    CHECK(getToken(&token));
    if (TTYPE==COMMA)
    CHECK(rdInParamsN())

    return EC_GOOD;
}

int rdInParamsN(){
    CHECK(getToken(&token));
    CHECK_R(TTYPE==IDENTIFIER || TTYPE==STRING || TTYPE==FLOAT || TTYPE==INTEGER,EC_SYN)
    CHECK(getToken(&token));
    if (TTYPE==COMMA)
    CHECK(rdInParamsN())

    return EC_GOOD;
}
