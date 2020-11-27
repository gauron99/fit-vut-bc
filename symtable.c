/**
        ~~ IFJ-20 ~~
    @file   symtable.c
    @brief  symbol table with hash fuction for faster data storage (using linked lists)  
    @date   30/10/20
    @author David Fridrich
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symtable.h"

extern symtableGI symGlobal;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int 
hashFunc(char *key){
    int final = 1;

    //cycle through the key by chars
    for (size_t i = 0; i < strlen(key); ++i)
    {
        //add a numerical value of given letter(ex.: 's' == 115)
        final += key[i];
    }
    //and finally devide the final number with modulo so it fits into the table and return
    return (final % SYMTABLE_SIZE);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


int
symtableItemInsert(char *funcKey, char *key, itemType type, char *value){
    symtableGlobalItem *func = symtableItemGetGlobal(funcKey);
    symtable tab = func->localTabs[func->countTabs-1];
    symtableItem *first,*temp;

    int hash = hashFunc(key);

    first = tab[hash];
    temp = first;
    //check if key is already in the table
    while(temp){
        //key is already in table, no need to do anything
        if(!strcmp(temp->key,key)){
            return SUCCESS;
        }
        temp=temp->next;
    }

    symtableItem *new = malloc(sizeof(symtableItem));
    if(!new){
        fprintf(stderr,">> Malloc in symtableItemInsert() failed\n");
        return INTERNAL_ERROR;
    }

    new->type = type;
    new->data = value;
    new->key = key;
    new->next = NULL; //if its the first one, it will be the end of the list and always point to NULL

    //insert item to the beggining of the list
    //if its not first insterted item in the list
    if(first){
        new->next = first;
    }

    func->localTabs[func->countTabs-1][hash] = new;
    return SUCCESS;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
symtableItem*
symtableItemGet(char *funcKey, char *key){
    symtableGlobalItem *func = symtableItemGetGlobal(funcKey);
    symtable *tab = &(func->localTabs[func->countTabs-1]);

    symtableItem *found;
    int hash = hashFunc(key);
    found = (*tab)[hash];
    while(found){
        if(!strcmp(key,found->key)){
            return found;
        }
        found=found->next;
    }
    //returns NULL when nothing is found
    return NULL;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void
printSymtable(char *funcKey){
    symtableGlobalItem *func = symtableItemGetGlobal(funcKey);
    symtable *tab = &(func->localTabs[func->countTabs-1]);
    int count = 0;

    // printf("Table daddy is %s\n",func->key);
    if(tab){
        symtableItem *temp;
        for (int i = 0; i < SYMTABLE_SIZE; ++i){
            temp = (*tab)[i];
            printf("        %d --- [ ",i);

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
        printf("        ~~~ # of items |%d| ~~~ ~~~\n\n",count);
    }
}

void
printAll(){
    symtableGI *tabtab = &symGlobal;
    symtableGlobalItem *item;
    int count = 0;
    printf("~~~ EVERYTHIN ~~~\n");
    for(int i = 0; i < SYMTABLE_SIZE;++i){
        item = (*tabtab)[i];
        printf("%d >> [ ",i);

        while(item){
            count++;
            if(item->next){
                printf("%s,",item->key);
            } else {
                printf("%s",item->key);
            }
            printf("\n          |");
            printf("\n----------local----------\n");
            printSymtable(item->key);
            item = item->next;
        }
        printf(" ]\n");
    }
    printf("~~~ # of items |%d| ~~~ ~~~\n\n",count);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//create global symtable
int
symtableCreateGlobal(symtableGI *tab){
    *tab = calloc(SYMTABLE_SIZE,sizeof(***tab));
    if(!tab){
        return INTERNAL_ERROR;
    } else {

        return SUCCESS;
    }
}

void
symtableDestroyGlobal(){

    if (symGlobal){
        symtableGI *tabtab = &symGlobal;

        symtableGlobalItem *delGl, *tempGl;
        //destroy global symtable
        for(int k = 0; k < SYMTABLE_SIZE; ++k){
            tempGl = (*tabtab)[k];
            while(tempGl){
                delGl = tempGl;
                tempGl = tempGl->next;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
                //free array of arg types if any was allocated
                if(delGl->countArgs){
                    free(delGl->args);
                }
                //free array of return types
                if(delGl->countRets){
                    free(delGl->returns);
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
                //destroy array of local symtables
                for (int j = delGl->countTabs; j > 0 ; --j)
                {
                    symtable *tab;
                    tab = &(delGl->localTabs[j-1]);
                    symtableItem *del,*temp;
                    //destroy one inner(local) symtable
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
                    (*tab)= NULL;
                }
                free(delGl->localTabs);
                free(delGl);
                delGl = NULL;
            }

        }
        free(symGlobal);
    }
}

int
symtableItemInsertGlobal(char *key){
    symtableGlobalItem *first,*temp;
    int hash = hashFunc(key);

    first = symGlobal[hash];
    temp = first;
    //check if key is already in the table
    while(temp){
        //key is already in table, no need to do anything
        if(!strcmp(temp->key,key)){
            return SUCCESS;
        }
        temp=temp->next;
    }

    symtableGlobalItem *new = malloc(sizeof(symtableGlobalItem));
    if(!new){
        fprintf(stderr,">> Malloc in symtableItemInsertGlobal() failed\n");
        return INTERNAL_ERROR;
    }

    new->key = key;
    //initialize values
    new->countArgs = 0;
    new->countRets = 0;
    new->countTabs = 1; // put one symtable in already

    new->localTabs = malloc(sizeof(symtable));
    if(!new->localTabs){
        fprintf(stderr,">> Malloc in symtableItemInsertGlobal() failed [symtable.c]\n");
        return INTERNAL_ERROR;
    }

    new->localTabs[0] = calloc(SYMTABLE_SIZE,sizeof(symtableItem));
    if(!new->localTabs[0]){
        fprintf(stderr,">> Calloc in symtableItemInsertGlobal() failed [symtable.c]\n");
        return INTERNAL_ERROR;
    }

    new->next = NULL; //if its the first one, it will be the end of the list and always point to NULL

    //insert item to the beggining of the list if its not first insterted item in the list
    if(first){
        new->next = first;
    }

    symGlobal[hash] = new;
    return SUCCESS;
}

symtableGlobalItem*
symtableItemGetGlobal(char *key){

    symtableGlobalItem *found;
    int hash = hashFunc(key);
    found = symGlobal[hash];
    while(found){
        if(!strcmp(key,found->key)){
            return found;
        }
        found=found->next;
    }
    //returns NULL when nothing is found
    return NULL;
}

int
pushArg(char *key, itemType type){
    symtableGI tab = symGlobal;

    bool found = false;
    int hash = hashFunc(key);
    symtableGlobalItem *item = tab[hash];

    while(item){
        if(!strcmp(key,item->key)){
            found = true;
            break;
        }
        item = item->next;
    }
    if(found){

        if(!item->countArgs){
            //first argument push
            item->args = malloc(2*sizeof(itemType));
            if(!item->args){
                fprintf(stderr,">> Error in pushArg() ~ malloc failed\n");
                return INTERNAL_ERROR;
            }
            item->args[0] = type;
        } else {
            item->args = realloc(item->args,(item->countArgs+2)*sizeof(itemType));
            if(!item->args){
                fprintf(stderr,">> Error in pushArg() ~ realloc failed\n");
                return INTERNAL_ERROR;
            }
            item->args[item->countArgs] = type; //there is not countArgs-1 because its not incremented yet
        }
        item->countArgs++;
        item->args[item->countArgs] = TYPE_NULL; //end can be checked by TYPE_NULL(-1) or countArgs

        return SUCCESS;
    }
    //didnt find element by key (function of name by key)
    printf("\n\nloudo you basic bitch, neumíš ani psát stringy už jo, sem tě vychoval teda jinak, soustřed se vole\n\n");
    return 666; //return error
}



int
pushRet(char *key,itemType type){
    symtableGI tab = symGlobal;

    bool found = false;
    int hash = hashFunc(key);
    symtableGlobalItem *item = tab[hash];

    while(item){
        if(!strcmp(key,item->key)){
            found = true;
            break;
        }
        item = item->next;
    }
    if(found){

        if(!item->countRets){
            //first argument push
            item->returns = malloc(2*sizeof(itemType));
            if(!item->returns){
                fprintf(stderr,">> Error in pushArg() ~ malloc failed\n");
                return INTERNAL_ERROR;
            }
            item->returns[0] = type;
        } else {
            item->returns = realloc(item->returns,(item->countRets+2)*sizeof(itemType));
            if(!item->returns){
                fprintf(stderr,">> Error in pushArg() ~ realloc failed\n");
                return INTERNAL_ERROR;
            }
            item->returns[item->countRets] = type;//no -1 because not incremented yet
        }
        item->countRets++;
        item->returns[item->countRets] = TYPE_NULL;

        return SUCCESS;
    }
    //didnt find element by key
    printf("loudo you basic bitch, neumíš ani psát stringy už jo, sem tě vychoval teda jinak, soustřed se vole\n");
    return 666; //return error
}


int
addScope(char *funcKey){
    symtableGlobalItem *func = symtableItemGetGlobal(funcKey);
    func->countTabs++;
    func->localTabs = realloc(func->localTabs, sizeof(symtable) * (func->countTabs));
    if(!func->localTabs){
        fprintf(stderr,">> Error in addScope(), realloc failed [symtable.c]\n");
        return INTERNAL_ERROR;
    }
    func->localTabs[func->countTabs-1] = calloc(SYMTABLE_SIZE,sizeof(symtableItem));
    if(!func->localTabs[func->countTabs-1]){
        fprintf(stderr,">> Error in addScope(), calloc failed[symtable.c]\n");
        return INTERNAL_ERROR;
    }
    return SUCCESS;
}

int
delScope(char *funcKey){
    symtableGlobalItem *func = symtableItemGetGlobal(funcKey);
    func->countTabs--;

    symtable *tab;
    tab = &(func->localTabs[func->countTabs]);
    symtableItem *del,*temp;
    //destroy one inner(local) symtable
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
    (*tab)= NULL;

    free(func->localTabs[func->countTabs]);
    func->localTabs = realloc(func->localTabs,sizeof(symtable)*func->countTabs);
    if(!func->localTabs){
        fprintf(stderr,">> Error, realloc failed in delScope() [symtable.c]\n");
        return INTERNAL_ERROR;
    }

    return SUCCESS;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// int
// main(void){
//     //create global symtable
//     symtableCreateGlobal(&symGlobal); //works

//     //insert items into global symtable (functions)
//     symtableItemInsertGlobal("main");
//     symtableItemInsertGlobal("loudikBoi");
//     symtableItemInsertGlobal("sabina_scenuje");
//     symtableItemInsertGlobal("MAAAAAACEEEEEEE"); //works
//     //add some arguments & retvals
//     if(pushArg("main",TYPE_INT64))return 1;
//     if(pushArg("main",TYPE_FLOAT64))return 1;
//     if(pushArg("main",TYPE_STRING))return 1;

//     if(pushArg("loudikBoi",TYPE_FLOAT64))return 1;
//     if(pushArg("sabina_scenuje",TYPE_BOOL))return 1;
//     if(pushArg("MAAAAAACEEEEEEE",TYPE_STRING))return 1; //works

//     // -- DEBUG
//     // symtableGlobalItem *item = symtableItemGetGlobal("main");
//     // printf("func: %s; countArgs: %d; args:",item->key,item->countArgs);
//     // for (int i = 0; i < item->countArgs; i++)
//     // {
//     //     if(i != (item->countArgs-1)){
//     //         printf("%d,", item->args[i]);
//     //     } else {
//     //         printf("%d\n", item->args[i]);
//     //     }
//     // } //works

//     if(pushRet("main",TYPE_BOOL))return 1;
//     if(pushRet("main",TYPE_STRING))return 1;
//     if(pushRet("main",TYPE_INT64))return 1;
//     if(pushRet("loudikBoi",TYPE_INT64))return 1;
//     if(pushRet("sabina_scenuje",TYPE_BOOL))return 1;
//     if(pushRet("MAAAAAACEEEEEEE",TYPE_STRING))return 1; //works

//     // -- DEBUG
//     // printf("should be 5,2,3 >> ");
//     // for (int i = 0; i < item->countRets; i++)
//     // {
//     //     printf(" %d,",item->returns[i]);
//     // } //works

//     // symtableGlobalItem *i = symtableItemGetGlobal("main");
//     itemType type = TYPE_INT64;
//     symtableItemInsert("main","variable",type,"5");
//     symtableItemInsert("main","variable2",type,"10");
//     symtableItemInsert("main","variable3",type,"18");

//     symtableItemInsert("loudikBoi","va1",type,"18");
//     symtableItemInsert("sabina_scenuje","ve1",type,"18");
//     symtableItemInsert("MAAAAAACEEEEEEE","aw2",type,"18");

//     // printSymtable("main");
//     printAll();

//     addScope("main");
//     symtableItemInsert("main","lalala",type,"18");
//     // symtableItem *it;
//     // it = symtableItemGet("main","lalala");
//     // printf("%s; %d\n",it->key,it->type);
//     printAll();

//     delScope("main");
//     printAll();

//     symtableDestroyGlobal();

//     // printf(">> %ld\n",sizeof(itemType));
//     return 0;
// }