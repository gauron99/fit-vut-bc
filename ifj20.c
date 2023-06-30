//
// Created by Burda Ludek on 11/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "ifj20.h"
#include "error.h"
#include "scanner.h"
#include "custom_string.h"
#include "parser.h"
#include "symtable.h"

symtableGI symGlobal;

int main(){

    //create global symtable
    symtableCreateGlobal(&symGlobal); 
    generateBeginningOfExistence();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    int lexicalResult = 0; //Scanner return code
    Token token;

    CHECK(prolog())

    return lexicalResult;
}
