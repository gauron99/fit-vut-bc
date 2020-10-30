/**
        ~~ IFJ-20 ~~
    @file   symtable.h
    @brief  function declarations for symtable.c 
    @date   30/10/20
    @author David Fridrich
*/

// to include header file only once
#ifndef SYMTABLE_H
#define SYMTABLE_H

//set to 10 just for example purpose, change later to 999 or something
#define SYMTABLE_SIZE 10

//to include error codes as return values for functions
#include "error.h"


/**
 * @brief structure for the item(node) in given linked list
 * 
 * key parameter holds value for each item to be identified with
 * data parameter holds data (gonna be changed depending on needs of others)
 * next pointer connects the linked list (pointer to the next item)
 */
typedef struct symtableItem{
    char *key;
    int data;
    struct symtableItem *next;

}symtableItem;

// ~~~~~~~~~~~~~~~~~ ??? is this even needed tho ??? ~~~~~~~~~~~~~~~~~ //

// //struct for all the pointers for linked_lists 
// typedef struct linked_list{

//     struct linked_list *next;

// }linked_list;


// pointer to the begining of the whole symtable thingy
typedef symtableItem **symtable;


//function prototypes //

/**
 * @brief create symtable
 * @param tab pointer to the whole symtable
 */
int
symtableCreate(symtable *tab);

/**
 * @brief destroy whole symtable
 *  destroys what can be destroyed, ignore otherwise
 * @param tab pointer to the whole symtable
 */
void
symtableDestroy(symtable *tab);

/**
 * @brief insert item into the symtable
 * @param tab pointer to the whole symtable
 * @param key value of the item (used in hashing)
 * @param inserted pointer to the item that has been inserted (pointer returned by argument)
 * @param data data to be stored in given item (to be changed, dependent on needs of others)
 */
int
symtableItemInsert(symtable *tab, char *key,symtableItem **inserted, int data);

/**
 * @brief destroy one item by given key value
 * @param tab pointer to the whole symtable
 * @param key value given by user, what node to destroy (if none found, do nothing)
 */
void
symtableItemDelete(symtable *tab, char *key);

/**
 * @brief get item from symtable by key
 * @param tab pointer to the whole symtable
 * @param found pointer to the desired item (pointer returned by argument)
 * @param key string identifying desired item to be found (if none found NULL is returned)
 */
int
symtableItemGet(symtable *tab, symtableItem **found, char *key);

#endif