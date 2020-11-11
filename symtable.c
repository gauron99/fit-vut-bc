/**
        ~~ IFJ-20 ~~
    @file   symtable.c
    @brief  symbol table with hash fuction for faster data storage (using linked lists)  
    @date   30/10/20
    @author David Fridrich
*/
//Yoyooo louda ma boi
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


int 
symtableCreate(symtable *tab){
    *tab = calloc(SYMTABLE_SIZE,sizeof(***tab));
    if(!tab){
        return INTERNAL_ERROR;
    } else {

        return SUCCESS;
    }

}


void 
symtableDestroy(symtable *tab){

    symtableItem *del,*temp;
    for (int i = 0; i < SYMTABLE_SIZE; ++i){
        temp = (*tab)[i];
        while(temp){
            del = temp;
            temp = temp->next;
            free(del);
        }
        (*tab)[i] = NULL;
    }
    free(*tab);
    *tab= NULL;
}


int 
symtableItemInsert(symtable *tab, char *key,symtableItem **inserted, int data){
    int hash = hashFunc(key);

    symtableItem *first,*temp;
    first = (*tab)[hash];
    temp = first;
    //check if key is already in the table
    while(temp){
        //key is already in table, no need to do anything
        if(!strcmp(temp->key,key)){
            //return pointer to the item anyways? //TODO
            *inserted = temp;
            return SUCCESS;
        }
        temp=temp->next;
    }

    symtableItem *new;
    new = malloc(sizeof(symtableItem));
    if(!new){
        fprintf(stderr,">> Malloc in symtableItemInsert() failed\n");
        return INTERNAL_ERROR;
    }
    
    //copy data inside the struct
    new->data = data;
    new->key = key;
    new->next = NULL; //if its the first one, it will be the end of the list and always point to NULL

    //insert item to the beggining of the list
    //if its not first insterted item in the list
    if(first){
        new->next = first;
    }

    (*tab)[hash] = new;
    //return pointer to newly created item by argument 'inserted'
    *inserted = new;
    return SUCCESS;
}


void 
symtableItemDelete(symtable *tab, char *key){
    int hash = hashFunc(key);
    symtableItem *todel,*next = NULL,*prev = NULL;
    todel = (*tab)[hash];

    while(todel){
        next = todel->next;

        if(!strcmp(todel->key,key)){
            free(todel);

            //if deleted node is the first one
            if(!prev){
                (*tab)[hash] = next;
                return;
            }
            prev->next = next;
            
        }
        prev = todel;
        todel = next;
    }
}

int 
symtableItemGet(symtable *tab, symtableItem **found, char *key){

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


void
printSymtable(symtable *tab){

    int count = 0;
    printf("~~~ Table ~~~\n");
    if (*tab){
        symtableItem *temp;
        for (int i = 0; i < SYMTABLE_SIZE; ++i){
            temp = (*tab)[i];
            printf("%d >> [ ",i);

            while(temp){
                count++;
                if(temp->next){
                    printf("%s,",temp->key);
                } else {
                    printf("%s",temp->key);
                    
                }
                temp = temp->next;
            }
            printf(" ]\n");
        }
        printf("~~~ # of items |%d| ~~~ ~~~\n\n",count);
    }    
}



int 
main(void){

    //create a symtable
    symtable tab;
    
    printf("~!~ Create symtable\n\n");
    //catch the return code (if calloc returned error)
    if(symtableCreate(&tab) != SUCCESS){

        fprintf(stderr,"chybka nebo co pri vytvareni tabulky\n");
        return 1;
    }
    
    printSymtable(&tab);

    symtableItem *item,*temp;

    printf("~!~ Insert one item\n\n");
    //item insertion
    symtableItemInsert(&tab,"bam",&item,10);

    printSymtable(&tab);

    printf("~!~ Insert items\n\n");
    // insert some items
    symtableItemInsert(&tab,"louda",&temp,69);
    symtableItemInsert(&tab,"hustoles",&temp,49);
    symtableItemInsert(&tab,"bramburka",&temp,49);
    symtableItemInsert(&tab,"bramburek",&temp,13);
    symtableItemInsert(&tab,"tri",&temp,565);
    symtableItemInsert(&tab,"osm",&temp,654);
    symtableItemInsert(&tab,"dva",&temp,96);
    symtableItemInsert(&tab,"faker",&temp,99);
    symtableItemInsert(&tab,"maker",&temp,444);
    symtableItemInsert(&tab,"baker",&temp,97);
    symtableItemInsert(&tab,"dejmivic",&temp,98);
    symtableItemInsert(&tab,"notakcoo",&temp,455);
    
    printSymtable(&tab);


    symtableItem *gimme;
    //get some
    printf("~!~ Get louda's value\n");
    symtableItemGet(&tab,&gimme,"louda");
    printf(" >>> node's key -- %s; his value -- %d\n\n",gimme->key,gimme->data);
    
    printf ("\n~!~ Delete bramburek,faker,dejmivic,baker~~~ \n");
    symtableItemDelete(&tab,"bramburek");
    symtableItemDelete(&tab,"faker");
    symtableItemDelete(&tab,"dejmivic");
    symtableItemDelete(&tab,"baker");
    printSymtable(&tab);

    printf ("\n~!~ Destroy all~~~ \n");
    symtableDestroy(&tab);
    printSymtable(&tab);

    return 0;
}