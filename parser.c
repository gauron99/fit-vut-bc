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
    instruct->name = name;
    instruct->boku = boku;
    instruct->no = no;
    instruct->pico = pico;
/*    strcpy(instruct->name, name);
    strcpy(instruct->boku,boku);
    strcpy(instruct->no,no);
    strcpy(instruct->pico,pico);*/
    generate(instruct);
    return;
}

int generate(trAK *instr){
    return 0;
}

int idSekv(int eos){
    int delim;
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    char** ids = malloc(sizeof(ids));
    int idCount = 0;
    ids[0] = calloc(1,strlen(TSTR));
    strcpy(ids[0], TSTR);

    CHECK(getToken(&token));

    while (TTYPE!=DEFINITION && TTYPE!=ASSIGNMENT){
        CHECK_R(TTYPE==COMMA,EC_SYN)
        CHECK(getToken(&token));

        CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
        ids = realloc(ids, sizeof(*ids)*(++idCount+1));
        if(!ids){
            fprintf(stderr,"NANI\n");
            return INTERNAL_ERROR;
        }
        ids[idCount] = calloc(1,strlen(TSTR));
        strcpy(ids[idCount], TSTR);
        CHECK(getToken(&token));
    }
    delim = TTYPE;
    if (((tokenType)delim) == DEFINITION){
        int checker = 0;
        for (int i = idCount; i >= 0; i--){
            if(!symtableItemGet(actualFunc->key,ids[i])) {
                assemble("DEFVAR",actualFunc->key,"","",instr);
                checker = 1;
            }
        }
        CHECK_R(checker,EC_SEM3);
    }
    else if (((tokenType)delim) == ASSIGNMENT){
        for (int i = idCount; i >= 0; i--)
            CHECK_R(symtableItemGet(actualFunc->key,ids[i]),EC_SEM3)
    }

    CHECK_R(TTYPE==((tokenType)delim),EC_SYN)

    /*CHECK(getToken(&token));

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    int tknCount = 2;
    CHECK(getToken(&token));

    if (TTYPE==LEFT_ROUND_BRACKET){
        while(TTYPE!=RIGHT_ROUND_BRACKET){
            CHECK(getToken(&token));
            tknCount++;
        }
        CHECK(getToken(&token));
        tknCount++;
    }*/

    int* expTypes = malloc(sizeof(int));
    int expTypCount = 0;
    expTypes[0] = analyzePrecedence();

    CHECK(getToken(&token));

    while (TTYPE==COMMA){
        CHECK(getToken(&token));

        expTypes = realloc(expTypes,sizeof(int)*(++expTypCount+1));
        if(!expTypes) {
            fprintf(stderr, "NANI\n");
            return INTERNAL_ERROR;
        }
        expTypes[expTypCount] = analyzePrecedence();

        CHECK(getToken(&token));
    }
    CHECK_R(expTypCount==idCount,EC_SEM6)

    if (idCount==0 && delim == (tokenType) DEFINITION)
        CHECK_R(!symtableItemGet(actualFunc->key,ids[0]),EC_SEM3)

    for (int i = idCount; i >= 0; i--){
        if (symtableItemGet(actualFunc->key,ids[i])) {
            symtableItem *tmp = symtableItemGet(actualFunc->key, ids[i]);
            CHECK_R(tmp->type == ((tokenType) expTypes[i]), EC_SEM6)
        }
        else
            symtableItemInsert(actualFunc->key,ids[i],(itemType) expTypes[i]);

        assemble("POPS","origShit","","",instr);
        assemble("MOVE",ids[i],"origShit","",instr);
    }

    CHECK_R(TTYPE==((tokenType)eos),EC_SYN)

    return EC_GOOD;
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
    int retType = 0;
    if (TTYPE==EOL_){
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"if"))){
        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)
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

        if (TTYPE!=SEMICOLON)
            CHECK(idSekv(SEMICOLON))

        CHECK_R(TTYPE==SEMICOLON,EC_SYN)

        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)

        CHECK(getToken(&token));
        CHECK_R(TTYPE==SEMICOLON,EC_SYN)

        CHECK(getToken(&token));
        if (TTYPE!=LEFT_CURLY_BRACKET)
            CHECK(idSekv(LEFT_CURLY_BRACKET))

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)
        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
        CHECK_R(TTYPE==RIGHT_CURLY_BRACKET,EC_SYN)
        CHECK(delScope(actualFunc->key))
        CHECK(delScope(actualFunc->key))
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !strcmp(TSTR,"return")){
        CHECK(getToken(&token));
        if (TTYPE!=EOL_){
            ungetToken(&token);
            int i = 0;
            do {
                CHECK(getToken(&token));
                CHECK_R(TTYPE==KEYWORD,EC_SYN)
                CHECK_R(actualFunc->returns[i] && switchToType(TSTR)==actualFunc->returns[i++],EC_SEM6)
                CHECK(getToken(&token));
                CHECK_R(TTYPE==EOL_ || TTYPE==COMMA,EC_SYN)
            } while (TTYPE!=EOL_);
        }
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==IDENTIFIER){
        Token tmp = token;
        CHECK(getToken(&token));
        if (TTYPE==COMMA || TTYPE == ASSIGNMENT || TTYPE == DEFINITION){
            ungetToken(&token);
            token = tmp;
            CHECK(idSekv(EOL_))
        }
        else if (TTYPE==LEFT_ROUND_BRACKET){
            CHECK(getToken(&token));
            if (TTYPE!=RIGHT_ROUND_BRACKET)
            CHECK(rdInParams())
            CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
            CHECK(getToken(&token));
        }
        else if (TTYPE==PLUS_EQ||TTYPE==MINUS_EQ||TTYPE==MUL_EQ||TTYPE==DIV_EQ){
            retType = analyzePrecedence();
            CHECK_R(retType>=0,(returnCode)-retType)
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
    int retType = 0;
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
        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)
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
    int retType = 0;
    if (TTYPE==IDENTIFIER){
        Token tmpToken;
        tmpToken = token;
        CHECK(getToken(&token));
        if (TTYPE!=LEFT_ROUND_BRACKET){
            ungetToken(&token);
            ungetToken(&tmpToken);
            retType = analyzePrecedence();
            CHECK_R(retType>=0,(returnCode)-retType)
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
        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)
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
    int retType = 0;
    retType = analyzePrecedence();
    CHECK_R(retType>=0,(returnCode)-retType)
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
