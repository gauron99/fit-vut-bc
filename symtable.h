/*
    @filename   symtable.h
*
    @brief  function declarations for symtable.c
*
    @date   21/10/20
* 
        IFJ-20
*/

// to include header file only once
#ifndef SYMTABLE_H
#define SYMTABLE_H

//set to 10 just for example purpose, change later to 999 or something
#define SYMTABLE_SIZE 10

//to include error codes as return values for functions
#include "error.h"


//structure for all the items(nodes) in given linked list
typedef struct symtable_item{
    char *key;
    int data;
    struct symtable_item *next;

}symtable_item;

// //struct for all the pointers for linked_lists 
// typedef struct linked_list{

//     struct linked_list *next;

// }linked_list;


// pointer to the begining of the whole symtable thingy
typedef symtable_item **symtable;


//function prototypes

//create the symtable
int symtableCreate(symtable *tab);
//destroy whole symtable
void symtableDestroy(symtable *tab);
//add item(node) to the symtable
int symtableItemInsert(symtable *tab, char *key, int data);
//delete item(node) from the symtable
void symtableItemDelete(symtable *tab, char *key);
//get an item(node) from the symtable
int symtableItemGet(symtable *tab, symtable_item **found, char *key);

#endif