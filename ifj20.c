//
// Created by louda on 11/10/20.
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

<<<<<<< HEAD
    //create global symtable
=======
     //create global symtable
>>>>>>> 9c35bd9b0620433e17a7b78e4710f8e272c46023
    symtableCreateGlobal(&symGlobal); //works

    //insert items into global symtable (functions)
    symtableItemInsertGlobal("main");
    symtableItemInsertGlobal("loudikBoi");
    symtableItemInsertGlobal("sabina_scenuje");
    symtableItemInsertGlobal("MAAAAAACEEEEEEE"); //works
<<<<<<< HEAD

=======
    
>>>>>>> 9c35bd9b0620433e17a7b78e4710f8e272c46023
    //add some arguments & retvals
    if(pushArg("main",TYPE_INT64))return 1;
    if(pushArg("main",TYPE_FLOAT64))return 1;
    if(pushArg("main",TYPE_STRING))return 1;

    if(pushArg("loudikBoi",TYPE_FLOAT64))return 1;
    if(pushArg("sabina_scenuje",TYPE_BOOL))return 1;
    if(pushArg("MAAAAAACEEEEEEE",TYPE_STRING))return 1; //works

    // -- DEBUG
    // symtableGlobalItem *item = symtableItemGetGlobal("main");
    // printf("func: %s; countArgs: %d; args:",item->key,item->countArgs);
    // for (int i = 0; i < item->countArgs; i++)
    // {
    //     if(i != (item->countArgs-1)){
    //         printf("%d,", item->args[i]);
    //     } else {
    //         printf("%d\n", item->args[i]);
    //     }
    // } //works

    if(pushRet("main",TYPE_BOOL))return 1;
    if(pushRet("main",TYPE_STRING))return 1;
    if(pushRet("main",TYPE_INT64))return 1;
    if(pushRet("loudikBoi",TYPE_INT64))return 1;
    if(pushRet("sabina_scenuje",TYPE_BOOL))return 1;
    if(pushRet("MAAAAAACEEEEEEE",TYPE_STRING))return 1; //works

    // -- DEBUG
    // printf("should be 5,2,3 >> ");
    // for (int i = 0; i < item->countRets; i++)
    // {
    //     printf(" %d,",item->returns[i]);
    // } //works

    // symtableGlobalItem *i = symtableItemGetGlobal("main");
    itemType type = TYPE_INT64;
<<<<<<< HEAD

=======
    
>>>>>>> 9c35bd9b0620433e17a7b78e4710f8e272c46023
    symtableItemInsert("main","variable",type,"5");
    symtableItemInsert("main","variable2",type,"10");
    symtableItemInsert("main","variable3",type,"18");

    symtableItemInsert("loudikBoi","va1",type,"18");
    symtableItemInsert("sabina_scenuje","ve1",type,"18");
    symtableItemInsert("MAAAAAACEEEEEEE","aw2",type,"18");

    // printSymtable("main");
    printAll();

    addScope("main");
    symtableItemInsert("main","lalala",type,"18");
    symtableItem *it;
    it = symtableItemGet("main","lalala");
    printf("THIS - %s; %d; %s\n",it->key,it->type, it->data);
    printAll();

    delScope("main");
    printAll();

    symtableDestroyGlobal();

    // printf(">> %ld\n",sizeof(itemType));
    return 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

<<<<<<< HEAD

=======
   
>>>>>>> 9c35bd9b0620433e17a7b78e4710f8e272c46023


    int lexicalResult = 0; //Scanner return code
    Token token;

    /*while(token.type != EOF_) {
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
    }*/
    CHECK(prolog())

    //printf("--- Status code: %d\n", lexicalResult);
    return lexicalResult;
    return 0;
}
