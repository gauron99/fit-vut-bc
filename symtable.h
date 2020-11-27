/**
        ~~ IFJ-20 ~~
    @file   symtable.h
    @brief  function declarations for symtable.c
    @date   23/11/20
    @author David Fridrich
*/

// to include header file only once
#ifndef SYMTABLE_H
#define SYMTABLE_H

//set to 10 just for example purpose, change later to 999 or something
#define SYMTABLE_SIZE 10



#include <stdbool.h>
//to include error codes as return values for functions
#include "error.h"

/**
 * @brief enum for item type used in symtable to identify what item is held
 */
typedef enum itemType{
    TYPE_NULL = -1, //not even needed maybea
    TYPE_INT64 = 2, //same values as in scanner.h
    TYPE_FLOAT64,
    TYPE_STRING,
    TYPE_BOOL,

}itemType;


/**
 * @brief structure for the item(node) in given linked list
 *
 * key parameter holds value for each item to be identified with
 * data parameter holds data (gonna be changed depending on needs of others)
 * next pointer connects the linked list (pointer to the next item)
 */

// pointer to the begining of the whole symtable thingy

typedef struct symtableItem{
    char *key;

    itemType type;

    struct symtableItem *next;

}symtableItem;

typedef symtableItem **symtable;

typedef struct symtableGlobalItem{
    char *key;

    itemType *args;
    int countArgs;

    itemType *returns;
    int countRets;

    symtable *localTabs;
    int countTabs;

    struct symtableGlobalItem *next;

}symtableGlobalItem;

typedef symtableGlobalItem **symtableGI;
//function prototypes start here //

/**
 * @brief insert item into the (inner) symtable
 * @param funcKey name of function (in higher symtable)
 * @param key value of the item (used in hashing)
 * @param type type of the item being inserted
 * @param value item value (int,float,string,bool...)
 */

int
symtableItemInsert(char *funcKey, char *key, itemType type);

/**
 * @brief get item from symtable by key
 * @param tab pointer to the whole symtable
 * @param key string identifying desired item to be found (if none found NULL is returned)
 * @return returns pointer to item found in symtable (by key) or NULL if item was not found
 */
symtableItem*
symtableItemGet(char *funcKey, char *key);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int
symtableCreateGlobal(symtableGI *tab);

int
symtableItemInsertGlobal(char *key);

symtableGlobalItem*
symtableItemGetGlobal(char *key);

void
symtableDestroyGlobal();

int
pushArg(char *key,itemType type);

int
pushRet(char *key,itemType type);

int
addScope(char *funcKey);

int
delScope(char *funcKey);

void
printAll();

void
printSymtable(char *funcKey);

#endif