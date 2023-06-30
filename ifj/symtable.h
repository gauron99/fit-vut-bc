/**
        ~~ IFJ-20 ~~
    @file   symtable.h
    @brief  function declarations for symtable.c
    @date   09/12/20
    @author David Fridrich
*/

// to include header file only once
#ifndef SYMTABLE_H
#define SYMTABLE_H

#define SYMTABLE_SIZE 997

#include <stdbool.h>
//to include error codes as return values for functions
#include "error.h"

/**
 * @brief enum for item type used in symtable to identify what item is held
 */
typedef enum itemType{
    TYPE_NULL = -1,
    TYPE_INT64 = 2, //same values as in scanner.h
    TYPE_FLOAT64,
    TYPE_STRING,
    TYPE_BOOL,

}itemType;

/**
 * @brief structure for the item(node) in given linked list
 * 
 * @param key key holds value for each item to be identified with
 * @param i int used to create unique identifiers for variables
 * @param type type holds value of given item type
 * @param next next points to next symtableItem struct in linked list
 */
typedef struct symtableItem{
    char *key;

    int i;
    itemType type;

    struct symtableItem *next;

}symtableItem;

/**
 * @brief pointer to symtable
 */
typedef symtableItem **symtable;

/**
 * @brief struct of Global items (functions)
 * @param args holds arguments of func
 * @param countArgs number of arguments
 * @param returns return values
 * @param countRets number of returns
 * @param localTabs pointer to scopes of function
 * @param countTabs number of scopes
 */
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

/**
 * @brief pointer to global symtable (symtable of functions)
 */
typedef symtableGlobalItem **symtableGI;
//function prototypes start here //

/**
 * @brief insert item into the (inner) symtable
 * @param funcKey name of function (in higher symtable)
 * @param key value of the item (used in hashing)
 * @param type type of the item being inserted
 * @param i unique identif for variables
 */
int
symtableItemInsert(char *funcKey, char *key, itemType type,int i);

/**
 * @brief destroy one item by given key value
 * @param tab pointer to the whole symtable
 * @param key value given by user, what node to destroy (if none found, do nothing)
 */
void//gone
symtableItemDelete(symtable *tab, char *key);

/**
 * @brief get item from symtable by key in all scopes
 * @param funcKey string identifying desired function to be found 
 * @param key string identifying desired item to be found; otherwise return NULL
 * @return returns pointer to item found in symtable (by key) or NULL if item was not found
 */
symtableItem*
symtableItemGet(char *funcKey, char *key);
/**
 * @brief get item from symtable by key only in actual scope
 * @param funcKey string identifying desired function to be found 
 * @param key string identifying desired item to be found; otherwise return NULL
 * @return returns pointer to item found in symtable (by key) or NULL if item was not found
 */
symtableItem*
symtableItemGetAct(char *funcKey, char *key);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/**
 * @brief init global symtable
 * @param tab pointer to the whole symtable
 * @return success or error
 */
int
symtableCreateGlobal(symtableGI *tab);
/**
 * @brief insert global item into the global symtable
 * @param key name of global item to be inserted
 * @return success or error
 */
int
symtableItemInsertGlobal(char *key);
/**
 * @brief get global item from global symtable by key; return NULL if not found
 * @param key points to item by its name
 * @return returns pointer to found item or NULL
 */
symtableGlobalItem*
symtableItemGetGlobal(char *key);
/**
 * @brief destroy the whole symtable
 */
void
symtableDestroyGlobal();
/**
 * @brief push argument into symtable
 * @returns success or error
 */
int
pushArg(char *key,itemType type);
/**
 * @brief push return value (add what function returns, if anything)
 * @return success or error
 */
int
pushRet(char *key,itemType type);
/**
 * @brief add scope to function given by funcKey
 * @param funcKey add scope to this function
 * @return success or error
 * 
 */
int 
addScope(char *funcKey);
/**
 * @brief deletes scope from a fuction
 * @param funcKey function name
 * @return success or error
 */
int
delScope(char *funcKey);

#endif