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
#include "symtable.h"

Token token;
extern symtable globalSymTab;
symtableGlobalItem *actualFunc;
trAK *instr;

void assemble(char* name, char* boku, char* no, char* pico, trAK *instruct) {
    strcpy(instruct->name, name);
    strcpy(instruct->boku,boku);
    strcpy(instruct->no,no);
    strcpy(instruct->pico,pico);
    generate(instruct);
    return;
}

int generate(trAK *instr){
    return 0;
}

int isType(char* string){
    if (!strcmp(string,"int") || !strcmp(string,"float64") || !strcmp(string,"string") || !strcmp(string,"bool"))
        return 1;
    return 0;
}

int switchToType(char* type){
    if (!strcmp(type,"string"))
        return TYPE_STRING;
    else if (!strcmp(type,"int"))
        return TYPE_INT64;
    else if (!strcmp(type,"float64"))
        return TYPE_FLOAT64;
    else if (!strcmp(type,"bool"))
        return TYPE_BOOL;
}

int prolog(){
    instr = malloc(sizeof(trAK));
    if (!instr){
        fprintf(stderr,"NANI\n");
        return INTERNAL_ERROR;
    }
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
    if (TTYPE==EOF_) {
        free(instr);
        return EC_GOOD; /// todo empty program?
    }
    else if (TTYPE == EOL_)
        CHECK(rdBody())
    else {
        CHECK_R(TTYPE==KEYWORD && !strcmp(TSTR,"func"),EC_SYN) // snad nebude padat se spatnym typem tokenu // cajk

        CHECK(rdDef())
        CHECK(rdBody())
    }
    return EC_GOOD;
}

int rdDef(){
    CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    CHECK(symtableItemInsertGlobal(TSTR))
    actualFunc = symtableItemGetGlobal(TSTR);

    assemble("LABEL",TSTR,"","",instr);

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
    char* varName;
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    strcpy(varName,TSTR);

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR)))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))



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
    char* varName;
    if (TTYPE==EOL_)
        CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    strcpy(varName,TSTR);

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR)))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))



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
    char* varName;
    if (TTYPE==IDENTIFIER){
        strcpy(varName,TSTR);

        CHECK(getToken(&token));
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

        CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR)))
        CHECK(pushRet(actualFunc->key,switchToType(TSTR)))


        CHECK(getToken(&token));
        if (TTYPE==COMMA)
            CHECK(rdReturnsNamedN())
    }
    else {
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)
        CHECK(pushRet(actualFunc->key,switchToType(TSTR)))

        CHECK(getToken(&token));
        if (TTYPE==COMMA)
        CHECK(rdReturnsN())
    }


    return EC_GOOD;
}

int rdReturnsNamedN(){
    char* varName;
    CHECK(getToken(&token));
    if (TTYPE==EOL_)
        CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    strcpy(varName,TSTR);

    CHECK(getToken(&token));

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR)))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))

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
    CHECK(pushRet(actualFunc->key,switchToType(TSTR)))

    CHECK(getToken(&token));

    if (TTYPE==COMMA)
        CHECK(rdReturnsN())

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

        CHECK(addScope(actualFunc->key))

        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
            CHECK(rdComm())

        CHECK(delScope(actualFunc->key))

        CHECK(getToken(&token));

        if (TTYPE!=EOL_)
            CHECK(rdElseOrNot())

        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"for"))){
        CHECK(addScope(actualFunc->key))
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
        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token))
        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));
        if(TTYPE!=RIGHT_CURLY_BRACKET)
            CHECK(rdComm())
        CHECK(delScope(actualFunc->key))
    }
    else {
        CHECK_R(TTYPE == KEYWORD && !strcmp(TSTR, "if"), EC_SYN)
        CHECK(analyzePrecedence());
        CHECK(getToken(&token));

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)
        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
            CHECK(rdComm())
        CHECK(delScope(actualFunc->key))
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
