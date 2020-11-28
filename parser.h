//
// Created by louda on 11/11/20.
//

#ifndef IFJ20_PARSER_H
#define IFJ20_PARSER_H

#include "symtable.h"

typedef struct trAK {
    char* name;

    char* boku;
    char* no;
    char* pico;
}trAK;

typedef struct calledFun{
    char* name;
    char** returns;

}calledFun;

int generate(trAK *instr);

void assemble(char* name, char* boku, char* no, char* pico, trAK *instr);


int isType(char* string);
int checkOriginality();
int idSekv(int delim, int eos);

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
int rdElseOrNot();
int rdIdsekv();
int rdExprSekv();
int rdExprsOrCall();
int rdInParams();
int rdInParamsN();

#endif //IFJ20_PARSER_H