//
// Created by Ludek Burda on 11/11/20.
//

#ifndef IFJ20_PARSER_H
#define IFJ20_PARSER_H

#include "symtable.h"
#include "scanner.h"
#include "generator.h"

void assemble(char* name, char* boku, char* no, char* pico, trAK *instr);

int isType(char* string);
int checkOriginality();
int idSekv(int eos);
int getToken(Token *token);
int loadFuncti();
int addInbuilt();

int prolog();
int rdBody();
int rdDef();
int rdParams();
int rdParamsN();
int rdReturns();
int rdReturnsN();
int rdReturnsNamed();
int rdReturnsNamedN();
int rdComm();
int rdElseOrNot(char *end);
int rdIdsekv();
int rdExprSekv();
int rdExprsOrCall();
int rdInParams();
int rdInParamsN();

#endif //IFJ20_PARSER_H