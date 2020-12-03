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
#include "generator.h"

Token token;
int isInFuncCall = 0;
int varCounter = 0;
extern symtable globalSymTab;
symtableGlobalItem *actualFunc;
trAK *instr;
Token *tknLoad;
int tknLoadCount = 0;

Token* tkns;
int ungot = 0;
int dewit = 1;

int ungetToken(Token *tk) {
    tkns[ungot++] = *tk;
    if (!(tkns = realloc(tkns, sizeof(Token)*(ungot+1))))
        return INTERNAL_ERROR;
}

int allocUnget(){
    if (!(tkns = malloc(sizeof(Token))))
        return INTERNAL_ERROR;
    if (!(tknLoad = malloc(sizeof(Token))))
        return INTERNAL_ERROR;
    return EC_GOOD;
}

int getToken(Token *tok){
    if(ungot && dewit) {
        *tok = tkns[0];
        for (int i = 1; i < ungot; i++){
            tkns[i-1] = tkns[i];
        }
        if (!(tkns = realloc(tkns, sizeof(Token)*(ungot--))))
            return INTERNAL_ERROR;
        return 0;
    }
    if(tknLoadCount) {
        *tok = tknLoad[0];
        for (int i = 1; i < tknLoadCount; i++){
            tknLoad[i-1] = tknLoad[i];
        }
        if (!(tknLoad = realloc(tknLoad, sizeof(Token)*(tknLoadCount--))))
            return INTERNAL_ERROR;
        return 0;
    }
}

int fillTknArr(Token *tok){
    tknLoad[tknLoadCount++] = *tok;
    if (!(tknLoad = realloc(tknLoad, sizeof(Token)*(tknLoadCount+1))))
        return INTERNAL_ERROR;
    return EC_GOOD;
}

int loadFuncti(){
    CHECK(gettToken(&token))
    while (token.type!=EOF_){
        CHECK(fillTknArr(&token))
        if (token.type == KEYWORD && !strcmp(token.value,"func")){
            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            CHECK_R(token.type==IDENTIFIER,EC_SYN)
            CHECK(symtableItemInsertGlobal(token.value))

            actualFunc = symtableItemGetGlobal(token.value);

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            CHECK_R(token.type==LEFT_ROUND_BRACKET,EC_SYN)

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))
            if (token.type!=RIGHT_ROUND_BRACKET)
            CHECK(rdParams())

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            if (token.type!=LEFT_CURLY_BRACKET)
            CHECK(rdReturns())

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            CHECK_R(token.type==EOL_,EC_SYN)
        }

        CHECK(gettToken(&token))
    }

    tknLoad[tknLoadCount++] = token;

    return EC_GOOD;
}

void assemble(char* name, char* boku, char* no, char* pico, trAK *instruct) {
    instruct->name = name;
    instruct->boku = boku;
    instruct->no = no;
    instruct->pico = pico;
    generate(*instruct);
    return;
}


int idSekv(int eos){
    int delim;
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)

    int wasFunCalled = 0;

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
                if (!strcmp(ids[i],"_"))
                    continue;
                checker = 1;
            }
        }
        CHECK_R(checker,EC_SEM3);
    }
    else if (((tokenType)delim) == ASSIGNMENT){
        for (int i = idCount; i >= 0; i--) {
            CHECK_R(symtableItemGet(actualFunc->key, ids[i]), EC_SEM3)
        }
    }

    CHECK_R(TTYPE==((tokenType)delim),EC_SYN)
    CHECK(getToken(&token));
    CHECK_R(TTYPE!=EOL_,EC_SYN)
    ungetToken(&token);
    int *expTypes = malloc(sizeof(int));
    int *argTypes = malloc(sizeof(int));
    int argTypCount = 0;
    int expTypCount = 0;
    int retType = 0;

    if (TTYPE == IDENTIFIER){
        dewit = 0;
        symtableGlobalItem *tempos = symtableItemGetGlobal(TSTR);
        CHECK(getToken(&token));
        ungetToken(&token);
        int tknCount = 2;

        if (TTYPE==LEFT_ROUND_BRACKET){
            wasFunCalled = 1;
            isInFuncCall = 1;
            while(TTYPE!=RIGHT_ROUND_BRACKET){
                CHECK(getToken(&token));
                ungetToken(&token);
                if (TTYPE==RIGHT_ROUND_BRACKET) {
                    tknCount++;
                    break;
                }
                CHECK_R(TTYPE==IDENTIFIER || TTYPE==FLOAT || TTYPE==INTEGER || TTYPE==STRING || TTYPE==BOOL,EC_SYN)
                retType = analyzePrecedence();
                CHECK_R(retType >= 0, (returnCode) -retType)
                CHECK_R((argTypes=realloc(argTypes,++argTypCount*sizeof(int))),EC_INTERNAL)
                argTypes[argTypCount-1]=retType;
                CHECK_R((tempos->args[argTypCount-1] && argTypes[argTypCount-1]==tempos->args[argTypCount-1]),EC_SEM6)
                CHECK(getToken(&token))
                ungetToken(&token);
                CHECK_R(TTYPE==COMMA || TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
                tknCount+=2;
            }
            isInFuncCall = 0;

            CHECK(getToken(&token));
            ungetToken(&token);
            if (TTYPE==EOL_) {
                CHECK_R(tempos->countRets == idCount+1, EC_SEM6)
                expTypCount=tempos->countRets-1;
                for (int i = 0; i < tempos->countRets;i++)
                    expTypes[i]=tempos->returns[i];
                dewit = 1;
                for (int i = 1; i<tknCount;i++)
                    getToken(&token);

            }
            else if (TTYPE==COMMA) {
                goto dere;
            }
            dewit = 1;
            for (int i = 1; i<tknCount;i++)
                getToken(&token);
        }
        else{
            dewit = 1;
            goto dere;
        }
        dewit = 1;
    }
    else {
        dere:
        retType = analyzePrecedence();
        CHECK_R(retType >= 0, (returnCode) -retType)
        expTypes[0] = retType;
        CHECK(getToken(&token));

        while (TTYPE == COMMA) {
            CHECK(getToken(&token));

            expTypes = realloc(expTypes, sizeof(int) * (++expTypCount + 1));
            if (!expTypes) {
                fprintf(stderr, "NANI\n");
                return INTERNAL_ERROR;
            }
            retType = analyzePrecedence();
            CHECK_R(retType >= 0, (returnCode) -retType)
            expTypes[expTypCount] = retType;

            CHECK(getToken(&token));
        }
    }
    if (wasFunCalled)
        CHECK_R(expTypCount==idCount,EC_SEM6)
        else
        CHECK_R(expTypCount==idCount,EC_SEM6)

    if (idCount==0 && delim == (tokenType) DEFINITION)
        CHECK_R(!symtableItemGet(actualFunc->key,ids[0]),EC_SEM3)
    for (int i = idCount; i >= 0; i--){
        if (symtableItemGet(actualFunc->key,ids[i])) {
            symtableItem *tmp = symtableItemGet(actualFunc->key, ids[i]);
            if (!strcmp("_",ids[i])){
                assemble("POPS","dev null","","",instr);
                continue;
            }
            if (wasFunCalled)
                CHECK_R(tmp->type == ((tokenType) expTypes[i]), EC_SEM6)
                else
                CHECK_R(tmp->type == ((tokenType) expTypes[i]), EC_SEM7)
        }
        else
            symtableItemInsert(actualFunc->key, ids[i], (itemType) expTypes[i], varCounter++);


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
    CHECK(allocUnget())
    CHECK(loadFuncti())
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
    actualFunc = symtableItemGetGlobal(TSTR);
    assemble("FUNC_DEF",TSTR,"","",instr);
    symtableItemInsert(actualFunc->key,"_",0,0);

    while (TTYPE!=LEFT_CURLY_BRACKET)
    CHECK(getToken(&token));

    CHECK(getToken(&token));
    CHECK_R(TTYPE==EOL_,EC_SYN)

    CHECK(getToken(&token));
    if (TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())
    assemble("FUNC_DEF_END","","","",instr);
    return EC_GOOD;
}

int rdParams(){
    char* varName = malloc(sizeof(TSTR));
    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    strcpy(varName,TSTR);

    CHECK(gettToken(&token))
    CHECK(fillTknArr(&token))

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR),varCounter++))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))

    CHECK(gettToken(&token))
    CHECK(fillTknArr(&token))
    if (TTYPE == COMMA) {
        CHECK(gettToken(&token))
        CHECK(fillTknArr(&token))
        CHECK(rdParamsN())
    }
    else
    CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdParamsN(){
    char* varName;
    if (TTYPE==EOL_) {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    varName = malloc(sizeof(TSTR));
    strcpy(varName,TSTR);

    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR),varCounter++))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))


    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))
    if (TTYPE==COMMA) {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
        CHECK(rdParamsN())
    }

    CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdReturns(){
    CHECK_R(TTYPE==LEFT_ROUND_BRACKET,EC_SYN)
    CHECK(gettToken(&token))
    CHECK(fillTknArr(&token))

    if (TTYPE!=RIGHT_ROUND_BRACKET) {
        CHECK(rdReturnsNamed())
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }
    else {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }

    CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

    return EC_GOOD;
}

int rdReturnsNamed(){
    char* varName;
    if (TTYPE==IDENTIFIER){
        strcpy(varName,TSTR);

        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

        CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR),varCounter++))
        CHECK(pushRet(actualFunc->key,switchToType(TSTR)))


        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
        if (TTYPE==COMMA)
        CHECK(rdReturnsNamedN())
    }
    else {
        CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)
        CHECK(pushRet(actualFunc->key,switchToType(TSTR)))

        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
        if (TTYPE==COMMA)
        CHECK(rdReturnsN())
    }


    return EC_GOOD;
}

int rdReturnsNamedN(){
    char* varName;
    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

    if (TTYPE==EOL_) {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }

    CHECK_R(TTYPE==IDENTIFIER,EC_SYN)
    strcpy(varName,TSTR);

    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)

    CHECK(symtableItemInsert(actualFunc->key,varName,switchToType(TSTR),varCounter++))
    CHECK(pushArg(actualFunc->key,switchToType(TSTR)))

    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

    if (TTYPE==COMMA)
    CHECK(rdReturnsNamedN())

    return EC_GOOD;
}

int rdReturnsN(){
    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

    if (TTYPE==EOL_) {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }

    CHECK_R(TTYPE==KEYWORD && isType(TSTR),EC_SYN)
    CHECK(pushRet(actualFunc->key,switchToType(TSTR)))

    CHECK(gettToken(&token));
    CHECK(fillTknArr(&token))

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
        getToken(&token);
        ungetToken(&token);
        CHECK_R(TTYPE!=SEMICOLON,EC_SYN)
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
                retType = analyzePrecedence();
                CHECK_R(retType>=0,(returnCode)-retType)
                CHECK_R(actualFunc->returns[i] && retType==actualFunc->returns[i++],EC_SEM6)
                CHECK(getToken(&token));

                CHECK_R(TTYPE==EOL_ || TTYPE==COMMA,EC_SYN)
            } while (TTYPE!=EOL_);
            CHECK_R(i==actualFunc->countRets,EC_SEM6)
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
            CHECK_R(!(symtableItemGet(actualFunc->key,tmp.value)),EC_SEM3)
            int *argTypes = malloc(sizeof(int));
            int argTypCount = 0;
            CHECK_R(symtableItemGetGlobal(tmp.value),EC_SEM3)
            symtableGlobalItem *glob = symtableItemGetGlobal(tmp.value);
            CHECK_R(glob->countRets==0,EC_SEM6)
            isInFuncCall = 1;
            while(TTYPE!=RIGHT_ROUND_BRACKET){
                getToken(&token);
                ungetToken(&token);
                CHECK_R(TTYPE==IDENTIFIER || TTYPE==FLOAT || TTYPE==INTEGER || TTYPE==STRING || TTYPE==BOOL,EC_SYN)
                retType = analyzePrecedence();
                CHECK_R(retType >= 0, (returnCode) -retType)
                CHECK_R((argTypes=realloc(argTypes,++argTypCount*sizeof(int))),EC_INTERNAL)
                argTypes[argTypCount-1]=retType;
                CHECK_R((glob->args[argTypCount-1] && argTypes[argTypCount-1]==glob->args[argTypCount-1]),EC_SEM6)
                CHECK(getToken(&token))
                CHECK_R(TTYPE==COMMA || TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
            }
            CHECK_R(glob->countArgs==argTypCount,EC_SEM6)
            isInFuncCall = 0;
            getToken(&token);
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
            ungetToken(&tmpToken);
            ungetToken(&token);
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

int addInbuilt(){
    CHECK(symtableItemInsertGlobal("inputs"))
    CHECK(pushRet("inputs",switchToType("string")))
    CHECK(pushRet("inputs",switchToType("int")))

    CHECK(symtableItemInsertGlobal("inputi"))
    CHECK(pushRet("inputi",switchToType("int")))
    CHECK(pushRet("inputi",switchToType("int")))

    CHECK(symtableItemInsertGlobal("inputf"))
    CHECK(pushRet("inputf",switchToType("float64")))
    CHECK(pushRet("inputf",switchToType("int")))

    CHECK(symtableItemInsertGlobal("print"))
    symtableGlobalItem *pls = symtableItemGetGlobal("print");
    pls->countArgs = -1;

    CHECK(symtableItemInsertGlobal("int2float"))
    CHECK(pushArg("int2float",switchToType("int")))
    CHECK(pushRet("int2float",switchToType("float64")))

    CHECK(symtableItemInsertGlobal("float2int"))
    CHECK(pushArg("int2float",switchToType("float64")))
    CHECK(pushRet("int2float",switchToType("int")))

    CHECK(symtableItemInsertGlobal("len"))
    CHECK(pushArg("len",switchToType("string")))
    CHECK(pushRet("len",switchToType("int")))

    CHECK(symtableItemInsertGlobal("substr"))
    CHECK(pushArg("substr",switchToType("string")))
    CHECK(pushArg("substr",switchToType("int")))
    CHECK(pushArg("substr",switchToType("int")))
    CHECK(pushRet("substr",switchToType("string")))
    CHECK(pushRet("substr",switchToType("int")))

    CHECK(symtableItemInsertGlobal("ord"))
    CHECK(pushArg("ord",switchToType("string")))
    CHECK(pushArg("ord",switchToType("int")))
    CHECK(pushRet("ord",switchToType("int")))
    CHECK(pushRet("ord",switchToType("int")))

    CHECK(symtableItemInsertGlobal("chr"))
    CHECK(pushArg("chr",switchToType("int")))
    CHECK(pushRet("chr",switchToType("string")))
    CHECK(pushRet("chr",switchToType("int")))

    return EC_GOOD;
}
