/*
    @filename   symtable.c
*
    @brief  symbol table with hash fuction for faster data storage (using linked lists)  
*
    @date   21/10/20
* 
        IFJ-20
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symtable.h"

int 
hashFunc(char *key){
    int final = 1;

    //cycle through the key by chars
    for (int i = 0; i < strlen(key); ++i)
    {
        //add a numerical value of given letter(ex.: 's' == 115)
        final += key[i];
    }
    //and finally devide the final number with modulo so it fits into the table and return
    return (final % SYMTABLE_SIZE);
}

//create the symtable
int 
symtableCreate(symtable *tab){
    *tab = calloc(SYMTABLE_SIZE,sizeof(***tab));
    if(!tab){
        return INTERNAL_ERROR;
    } else {

        return SUCCESS;
    }

}
//delete everything ~ whole symtable
void 
symtableDestroy(symtable *tab){

    symtable_item *del,*temp;
    for (int i = 0; i < SYMTABLE_SIZE; ++i)
    {
        temp = (*tab)[i];
        while(temp){
            del = temp;
            temp = temp->next;
            free(del);
        }
    }
    free(*tab);
}


/*
 * add item(node) to the symtable
 * if the key is already in table, do nothing, return SUCCESS 
 * if key is not in the table, add it, return SUCCESS
 * 
*/
int 
symtableItemInsert(symtable *tab, char *key, int data){
    int hash = hashFunc(key);

    symtable_item *temp;
    temp = (*tab)[hash];

    //check if key is already in the table
    while(temp){
        //key is already in table, no need to do anything
        if(!strcmp(temp->key,key)){
            return SUCCESS;
        }
        temp=temp->next;
    }

    symtable_item *new;
    new = malloc(sizeof(symtable_item));
    if(!new){
        fprintf(stderr,">> Malloc in symtable_item_insert() failed\n");
        return INTERNAL_ERROR;
    }
    
    //copy data inside the struct
    new->data = data;
    new->key = key;
    new->next = NULL; //if its the first one, it will be the end of the list and always point to NULL

    //insert item to the beggining of the list
    //if its not first insterted item in the list
    if(temp){
        new->next = temp;
    }

    (*tab)[hash] = new;

    return SUCCESS;
}


/*
 * delete item by given key
 * if item was found, delete it
 * if not, do nothing
 */
void 
symtableItemDelete(symtable *tab, char *key){
    int hash = hashFunc(key);
    symtable_item *todel,*next = NULL,*prev = NULL;
    todel = (*tab)[hash];

    while(todel){
        next = todel->next;
        if(!strcmp(todel->key,key)){
            free(todel);

            //if deleted node is the first one
            if(!prev){
                (*tab)[hash] = next;
            } else {// else its not
                prev->next = next;
            }
            return;
        }
        prev = todel;
        todel = next;
    }
}


//get an item(node) from the symtable
int 
symtableItemGet(symtable *tab, symtable_item **found, char *key){

    int hash = hashFunc(key);
    *found = (*tab)[hash];
    while(*found){
        if(!strcmp(key,(*found)->key)){
            return SUCCESS;
        }
        *found=(*found)->next;
    }
    //returns *found as NULL when nothing is found (reaches the end of the list)
    return SUCCESS;
}

/*
void
printSymtable(symtable *tab){
    
}
*/


int 
main(void){

    symtable tab;
    int retcode = symtableCreate(&tab);

    symtableItemInsert(&tab,"bam",10);
    symtable_item *item;
    symtableItemGet(&tab,&item,"bam");
    printf("should be 10 ->> |%d|\n",item->data);

    if(retcode == 0){
        printf("all good\n");
        return 0;
    } else {
        printf("not good\n");
        return retcode;
    }
}