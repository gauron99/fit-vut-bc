//
// Created by Ludek Burda on 11/11/20.
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

Token* tkns = NULL; //kuli was here
int ungot = 0;
int dewit = 1;

int loudaFirstBoi = 1; //kuli was here


/// funkce umoznujici nahlizeni na dalsi token ///
int ungetToken(Token *tk) {
    tkns[ungot++] = *tk;
    if (!(tkns = realloc(tkns, sizeof(Token)*(ungot+1))))
        return INTERNAL_ERROR;
}

/// funkce alokujici misto pro strukturu jenz vyuziva predesla funkce ///
int allocUnget(){
    if (!(tkns = malloc(sizeof(Token))))
        return INTERNAL_ERROR;
    if (!(tknLoad = malloc(sizeof(Token))))
        return INTERNAL_ERROR;
    tknLoad->value = NULL; //kuli was here
    tkns->value = NULL; //kuli was here
    return EC_GOOD;
}

/// funkce nacitajici token bud z pole tokenu nacteneho pri prvnim pruchodu kodu, pripadne z pole tokenu jenz vyuziva funkce pro nahlizeni na dalsi tokeny ///
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

/// funkce pro nacteni tokenu pri prvnim pruchodu ///
int fillTknArr(Token *tok){
    tknLoad[tknLoadCount++] = *tok;
    if (!(tknLoad = realloc(tknLoad, sizeof(Token)*(tknLoadCount+1))))
        return INTERNAL_ERROR;
    return EC_GOOD;
}

/// funkce jenz nacte definice funkci a jejich argumenty a navratove typy pri prvnim pruchodu ///
int loadFuncti(){
    CHECK(gettToken(&token))
    while (token.type!=EOF_){
        CHECK(fillTknArr(&token))
        if (token.type == KEYWORD && !strcmp(token.value,"func")){
            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            CHECK_R(token.type==IDENTIFIER,EC_SYN)
            CHECK_R(!symtableItemGetGlobal(token.value),EC_SEM3)
            CHECK(symtableItemInsertGlobal(token.value))

            actualFunc = symtableItemGetGlobal(token.value);

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))

            CHECK_R(token.type==LEFT_ROUND_BRACKET,EC_SYN)

            CHECK(gettToken(&token));
            CHECK(fillTknArr(&token))
            if (token.type!=RIGHT_ROUND_BRACKET) {
                CHECK_R(strcmp(actualFunc->key,"main"),EC_SEM6)
                CHECK(rdParams())
            }

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
    CHECK_R(symtableItemGetGlobal("main"),EC_SEM3)

    tknLoad[tknLoadCount++] = token;

    return EC_GOOD;
}

/// funkce jenz sesklada strukturu pro 3AK a odesle ji generatoru ///
void assemble(char* name, char* boku, char* no, char* pico, trAK *instruct) {
    instruct->name = name;
    instruct->boku = boku;
    instruct->no = no;
    instruct->pico = pico;
    generate(*instruct);
    return;
}

/// funkce nahrazujici rekurzivni sestup pro nacitani sekvence promennych ///
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
            if(!symtableItemGetAct(actualFunc->key,ids[i])) {
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
        Token tmp = token;
        dewit = 0;
        symtableGlobalItem *tempos = symtableItemGetGlobal(TSTR);
        CHECK(getToken(&token));
        ungetToken(&token);
        int brCount = 1;
        if (TTYPE==LEFT_ROUND_BRACKET){
            isInFuncCall = 1;
            wasFunCalled = 1;
            while(brCount!=0){
                CHECK(getToken(&token));
                ungetToken(&token);
                if (TTYPE==LEFT_ROUND_BRACKET)
                    brCount++;
                if (TTYPE==RIGHT_ROUND_BRACKET)
                    brCount--;
                if(TTYPE==EOF_)
                    return EC_SEM7;
            }
            getToken(&token);
            ungetToken(&token);
            if (TTYPE==EOL_) {
                dewit = 1;
                getToken(&token);
                CHECK_R(symtableItemGetGlobal(TSTR),EC_SEM3)
                getToken(&token);
                CHECK_R(TTYPE==LEFT_ROUND_BRACKET,EC_SYN)
                getToken(&token);
                if (TTYPE!=RIGHT_ROUND_BRACKET){
                    if (!(tkns = realloc(tkns, sizeof(Token)*(ungot+1))))
                        return INTERNAL_ERROR;
                    for (int oops = ungot; 0<oops; oops--){
                        tkns[oops] = tkns[oops-1];
                    }
                    tkns[0] = token;
                    ungot++;

                    do {
                        retType = analyzePrecedence();
                        CHECK_R(retType >= 0, (returnCode) -retType)
                        CHECK_R((argTypes=realloc(argTypes,++argTypCount*sizeof(int))),EC_INTERNAL)
                        argTypes[argTypCount-1]=retType;
                        CHECK_R(tempos->countArgs>=argTypCount,EC_SEM6)
                        CHECK_R(argTypes[argTypCount-1]==tempos->args[argTypCount-1],EC_SEM6)
                        token = tkns[--ungot];
                        CHECK_R(TTYPE==COMMA || TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
                    } while (TTYPE==COMMA);
                    CHECK_R(TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
                }
                isInFuncCall = 0;
                getToken(&token);

                CHECK_R(tempos->countRets == idCount+1, EC_SEM6)
                expTypCount=tempos->countRets-1;
                for (int i = 0; i < tempos->countRets;i++){
                    expTypes[i]=tempos->returns[i];
                }
                dewit = 1;
            }
            else {
                dewit = 1;
                goto dere;
            }
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
        CHECK_R(!symtableItemGetAct(actualFunc->key,ids[0]),EC_SEM3)

    CHECK_R(idCount==expTypCount,EC_SEM7)

    for (int i = idCount; i >= 0; i--){
        char *name = malloc(11+strlen(ids[i]));
        char *theInt = malloc(10);
        char *pls = malloc(11+strlen(ids[i])+3);
        strcpy(pls,"LF@");
        if (symtableItemGetAct(actualFunc->key,ids[i])) {
            symtableItem *tmp = symtableItemGet(actualFunc->key, ids[i]);
            if(!sprintf(theInt,"%d",tmp->i))
                return INTERNAL_ERROR;
            strcpy(name,ids[i]);
            name = strcat(name,"$");
            name = strcat(name,theInt);
            strcat(pls,name);
            if (!strcmp("_",ids[i])){
                assemble("POPS","dev null","","",instr);
                continue;
            }
            if (wasFunCalled) {
                CHECK_R(tmp->type == (tokenType) expTypes[i], EC_SEM6)
            }
            else {
                CHECK_R(tmp->type == (tokenType) expTypes[i], EC_SEM7)
            }

        }
        else {
            if(!sprintf(theInt,"%d",varCounter))
                return INTERNAL_ERROR;
            strcpy(name,ids[i]);
            name = strcat(name,"$");
            name = strcat(name,theInt);
            strcat(pls,name);
            assemble("DEFVAR",pls,"","",instr);
            symtableItemInsert(actualFunc->key, ids[i], (itemType) expTypes[i], varCounter++);
        }
        assemble("POPS",pls,"","",instr);
    }

    CHECK_R(TTYPE==((tokenType)eos),EC_SYN)
    return EC_GOOD;
}
/// funkce jenz zkontroluje zda je string jednim z datovych typu ifj20 ///
int isType(char* string){
    if (!strcmp(string,"int") || !strcmp(string,"float64") || !strcmp(string,"string") || !strcmp(string,"bool"))
        return 1;
    return 0;
}
/// funkce jenz prevede datovy typ ve formatu stringu na lokalni enum ///
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
/// zakladni funkce jenz ridi chod celeho programu ///
int prolog(){
    if(loudaFirstBoi){ //kuli was here
        instr = malloc(sizeof(trAK));
        addInbuilt();
        CHECK(allocUnget())
        CHECK(loadFuncti())
        loudaFirstBoi = 0; //kuli was here
    }
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
/// funkce rekurzivniho sestupu kontrolujici telo funkce ///
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
/// funkce rekurzivniho sestupu pri definici funkce ///
int rdDef(){
    char** args = malloc(sizeof(char*));
    int* rets = malloc(sizeof(int));
    int argCount = 0;
    CHECK(getToken(&token));
    actualFunc = symtableItemGetGlobal(TSTR);
    assemble("FUNC_DEF",TSTR,"","",instr);
    symtableItemInsert(actualFunc->key,"_",0,0);

    getToken(&token);

    while (TTYPE!=RIGHT_ROUND_BRACKET){
        getToken(&token);
        if(!(args = realloc(args,sizeof(char*) * (argCount+1))))
            return INTERNAL_ERROR;
        if(!(rets = realloc(rets,sizeof(char*) * (argCount+1))))
            return INTERNAL_ERROR;
        if (TTYPE==RIGHT_ROUND_BRACKET)
            break;
        args[argCount] = malloc(strlen(TSTR));
        args[argCount++] = TSTR;
        getToken(&token);
        rets[argCount-1] = switchToType(TSTR);
        getToken(&token);
    }
    for (int i = argCount-1; i >= 0; i--){
        symtableItem *it = symtableItemGet(actualFunc->key,args[i]);
        char *name = malloc(11+strlen(args[i]));
        char *theInt = malloc(10);
        if(!sprintf(theInt,"%d",it->i))
            return INTERNAL_ERROR;
        name = args[i];
        name = strcat(name,"$");
        name = strcat(name,theInt);
        char *pls = malloc(strlen(name)+3);
        strcpy(pls,"LF@");
        strcat(pls,name);
        assemble("DEFVAR",pls,"","",instr);
        assemble("POPS",pls,"","",instr);
    }
    while (TTYPE!=LEFT_CURLY_BRACKET)
        getToken(&token);

    CHECK(getToken(&token));
    CHECK_R(TTYPE==EOL_,EC_SYN)

    CHECK(getToken(&token));
    if (TTYPE!=RIGHT_CURLY_BRACKET)
    CHECK(rdComm())
    assemble("FUNC_DEF_END",actualFunc->key,"","",instr);
    return EC_GOOD;
}
/// funkce rekurzivniho sestupu pri nacitani parametru funkce ///
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

/// funkce rekurzivniho sestupu rozvijejici predchozi funkci ///
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

/// funkce rekurzivniho sestupu nacitajici definovane navratove hodnoty funkce ///
int rdReturns(){
    CHECK_R(TTYPE==LEFT_ROUND_BRACKET,EC_SYN)
    CHECK(gettToken(&token))
    CHECK(fillTknArr(&token))

    if (TTYPE!=RIGHT_ROUND_BRACKET){
        CHECK_R(strcmp(actualFunc->key,"main"),EC_SEM6)
        CHECK(rdReturnsNamed())
        CHECK(gettToken(&token))
        CHECK(fillTknArr(&token))
    }
    else {
        CHECK(gettToken(&token));
        CHECK(fillTknArr(&token))
    }

    CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

    return EC_GOOD;
}

/// funkce rekurzivniho sestupu rozvijejici predchozi funkci ///
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

/// funkce rekurzivniho sestupu rozvijejici predchozi funkci ///
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

/// funkce rekurzivniho sestupu rozvijejici funkci pro nacitani navratovych hodnot ///
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

/// funkce rekurzivniho sestupu pro kontrolu prikazu ifj20 ///
int rdComm(){
    int retType = 0;
    if (TTYPE==EOL_){
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"if"))){
        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)
        CHECK_R(retType==BOOL,EC_SEM7)
        char *next = generate_label();
        char *end = generate_label();
        assemble("START_IF",next,"","",instr);
        CHECK(getToken(&token));

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)

        CHECK(addScope(actualFunc->key))

        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())

        assemble("JUMP",end,"","",instr);
        assemble("LABEL",next,"","",instr);
        CHECK(delScope(actualFunc->key))

        CHECK(getToken(&token));

        if (TTYPE!=EOL_)
        CHECK(rdElseOrNot(end))
        assemble("LABEL",end,"","",instr);
        CHECK(getToken(&token));
        CHECK(rdComm())
    }
    else if (TTYPE==KEYWORD && !(strcmp(TSTR,"for"))){
        char *for_start = generate_label();
        char *za_forem = generate_label();
        char *for_body = generate_label();
        char *aktualizace = generate_label();

        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token));

        if (TTYPE!=SEMICOLON)
        CHECK(idSekv(SEMICOLON))
        CHECK_R(TTYPE==SEMICOLON,EC_SYN)
        assemble("LABEL",for_start,"","",instr);
        getToken(&token);
        ungetToken(&token);
        CHECK_R(TTYPE!=SEMICOLON,EC_SYN)
        retType = analyzePrecedence();
        CHECK_R(retType>=0,(returnCode)-retType)
        CHECK_R(retType==BOOL,EC_SEM5)
        assemble("FOR_COND",za_forem,for_body,"",instr);
        CHECK(getToken(&token));
        CHECK_R(TTYPE==SEMICOLON,EC_SYN)

        CHECK(getToken(&token));
        assemble("LABEL",aktualizace,"","",instr);
        if (TTYPE!=LEFT_CURLY_BRACKET)
            CHECK(idSekv(LEFT_CURLY_BRACKET))
        assemble("FOR_FINISH",for_start,for_body,"",instr);

        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)
        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token));
        CHECK_R(TTYPE==EOL_,EC_SYN)
        CHECK(getToken(&token));
        CHECK(rdComm())
        CHECK_R(TTYPE==RIGHT_CURLY_BRACKET,EC_SYN)
        CHECK(delScope(actualFunc->key))
        CHECK(delScope(actualFunc->key))
        assemble("FOR_FINISH",aktualizace,za_forem,"",instr);
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
            if (!strcmp(tmp.value,"print")){
                isInFuncCall = 1;
                while (TTYPE!=RIGHT_ROUND_BRACKET){
                    retType = analyzePrecedence();
                    assemble("CALL","print","","",instr);
                    getToken(&token);
                    CHECK_R(TTYPE==COMMA || TTYPE==RIGHT_ROUND_BRACKET,EC_SYN)
                }
                isInFuncCall = 0;
            }
            else {
                CHECK_R(!(symtableItemGet(actualFunc->key, tmp.value)), EC_SEM3)
                int *argTypes = malloc(sizeof(int));
                int argTypCount = 0;
                CHECK_R(symtableItemGetGlobal(tmp.value), EC_SEM3)
                symtableGlobalItem *glob = symtableItemGetGlobal(tmp.value);
                CHECK_R(glob->countRets == 0, EC_SEM6)
                isInFuncCall = 1;
                while (TTYPE != RIGHT_ROUND_BRACKET) {
                    getToken(&token);
                    ungetToken(&token);
                    CHECK_R(TTYPE == IDENTIFIER || TTYPE == FLOAT || TTYPE == INTEGER || TTYPE == STRING ||
                            TTYPE == BOOL, EC_SYN)
                    retType = analyzePrecedence();
                    CHECK_R(retType >= 0, (returnCode) -retType)
                    CHECK_R((argTypes = realloc(argTypes, ++argTypCount * sizeof(int))), EC_INTERNAL)
                    argTypes[argTypCount - 1] = retType;
                    CHECK_R(argTypCount==glob->countArgs,EC_SEM6)
                    CHECK_R(argTypes[argTypCount - 1] == glob->args[argTypCount - 1],EC_SEM6)
                    CHECK(getToken(&token))
                    CHECK_R(TTYPE == COMMA || TTYPE == RIGHT_ROUND_BRACKET, EC_SYN)
                }
                assemble("CALL", tmp.value, "", "", instr);
                CHECK_R(glob->countArgs == argTypCount, EC_SEM6)
                isInFuncCall = 0;
            }
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

/// funkce rekurzivniho sestupu pro kontrolu else navesti ///
int rdElseOrNot(char *end){
    int retType = 0;
    char *next = generate_label();
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
        assemble("START_IF",next,"","",instr);
        CHECK_R(TTYPE==LEFT_CURLY_BRACKET,EC_SYN)
        CHECK(addScope(actualFunc->key))
        CHECK(getToken(&token));

        CHECK_R(TTYPE==EOL_,EC_SYN)

        CHECK(getToken(&token));

        if (TTYPE!=RIGHT_CURLY_BRACKET)
        CHECK(rdComm())
        assemble("JUMP",end,"","",instr);
        assemble("LABEL",next,"","",instr);
        CHECK(delScope(actualFunc->key))
        CHECK(getToken(&token));

        if (TTYPE!=EOL_)
        CHECK(rdElseOrNot(end))
    }

    return EC_GOOD;
}

/// funkce jenz ulozi do tabulky symbolu informace o vestavenych funkcich ifj20 ///
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
    CHECK(pushArg("float2int",switchToType("float64")))
    CHECK(pushRet("float2int",switchToType("int")))

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