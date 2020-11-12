//
// Created by louda on 11/11/20.
//

#ifndef IFJ20_PARSER_H
#define IFJ20_PARSER_H

int isType(char* string);

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

int precedence();

#endif //IFJ20_PARSER_H
