
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
	
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/

    if(L->First){
        tDLElemPtr temp;
        while (L->First != L->Last){
            temp = L->First;
            L->First = L->First->rptr;

            temp->lptr = NULL;
            temp->rptr = NULL;

            L->First->lptr = NULL;

            free(temp);
        }
        temp = L->First;
        free(temp);

        L->First = NULL;
        L->Last = NULL;
        L->Act = NULL;
    }
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr new = malloc(sizeof(tDLElemPtr));
	if (!new){
        DLError();
        return;
    }

    new->data = val;
    new->lptr = NULL;

    //if it is the first node
    if(!L->First){
        L->First = new;
        L->Last = new;

        L->Last->rptr = NULL;
        L->First->lptr = NULL;
        
        new->rptr = NULL;
    }
    else{
        new->rptr = L->First;
        L->First->lptr = new;
        L->First = new;
    }
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr new = malloc(sizeof(tDLElemPtr));
	if (!new){
        DLError();
        return;
    }

    new->data = val;
    new->rptr = NULL;

    //if it is the first node
    if(!L->First){
        L->First = new;
        L->Last = new;

        L->Last->rptr = NULL;
        L->First->lptr = NULL;

        new->lptr = NULL;
    }
    else{
        new->lptr = L->Last;
        L->Last->rptr = new;
        L->Last = new;
    }
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if(!L->First){
        DLError();
        return;
    }

    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
    if(!L->First){
        DLError();
        return;
    }

    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if(L->First){

    //theres only one node in the whole list
        if(L->First == L->Last){
            free(L->Last);
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
        }
        //theres more than one node in the list
        else{
            //lose activity
            if(L->First == L->Act){
                L->Act = NULL;
            }

            tDLElemPtr temp = L->First;
            L->First = L->First->rptr;
            L->First->lptr = NULL;

            //TODO
            // temp->rptr = NULL;
            // temp->lptr = NULL;
            free(temp);
        }       
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
	
    if(L->First){
        //theres only one node in the whole list
        if(L->First == L->Last){
            free(L->Last);
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
        }
        else{
            tDLElemPtr temp = L->Last;
            L->Last = L->Last->lptr;
            L->Last->rptr = NULL;

            //TODO
            // temp->rptr = NULL;
            // temp->lptr = NULL;
            free(temp);
        }
	}	
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	//node has to be active and NOT the last one
    if(L->Act && (L->Act != L->Last)){
        tDLElemPtr todel = L->Act->rptr;

        //if its the last node set L->Last to the previous one
        if(todel == L->Last){
            L->Last = L->Act;
            L->Last->rptr = NULL;
        }
        else{
            //connect nodes around the node to be deleted
            L->Act->rptr = todel->rptr;
            todel->rptr->lptr = L->Act;

        }
        //TODO
        // todel->lptr = NULL;
        // todel->rptr = NULL;
        free(todel);
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if(L->Act && (L->Act != L->First)){
        tDLElemPtr todel = L->Act->lptr;

        if(todel == L->First){
            L->First = L->Act;
            L->First->lptr = NULL;
        }
        else{
            L->Act->lptr = todel->lptr;
            todel->lptr->rptr = L->Act;
        }

        //TODO
        // todel->lptr = NULL;
        // todel->rptr = NULL;
        free(todel);
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    //list must be active, otherwise do nothing
    if(L->Act){
        tDLElemPtr new = malloc(sizeof(tDLElemPtr));
        if (!new){
            DLError();
            return;
        }
        new->data = val;

        //if active node is also the last node we have to shift L->Last
        if(L->Act == L->Last){
            //connect right pointer of current Last node to the newly created one
            L->Last->rptr = new;
            //connect left pointer of newly created node with the currently Last node
            new->lptr = L->Last;
            //set right pointer of the new one to NULL hence its gonna be the new Last node
            new->rptr = NULL;
            //set the new node as the Last node
            L->Last = new;
        }
        //if currently active node is NOT the last node (its somewhere in the middle/first)
        else{
            //save next node as 'temp'
            tDLElemPtr temp = L->Act->rptr;
            //connect temp's left pointer to the new node
            temp->lptr = new;
            //set new node's right pointer to the 'temp' node (AKA now new & node to it's right are connected)
            new->rptr = temp;

            //set right pointer of Active node to point to the newly created one
            L->Act->rptr = new;
            //set new node's left pointer to the active node (hence active & new nodes are connected)
            new->lptr = L->Act;
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    //list must be active, otherwise do nothing
    if(L->Act){
        tDLElemPtr new = malloc(sizeof(tDLElemPtr));
        if (!new){
            DLError();
            return;
        }
        new->data = val;

        if(L->Act == L->First){
            L->First->lptr = new;
            new->rptr = L->First;
            new->lptr = NULL;
            L->First = new;
        }
        else{
            tDLElemPtr temp = L->Act->lptr;

            temp->rptr = new;
            new->lptr = temp;

            L->Act->lptr = new;
            new->rptr = L->Act;
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
	if(L->Act){
        *val = L->Act->data;
    }
    else{
        DLError();
    }
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act){
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act){
       if(L->Act == L->Last){
            L->Act = NULL;
        }
        else{
            L->Act = L->Act->rptr;
        }
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act){
        if(L->Act == L->First){
            L->Act = NULL;
        }
        else{
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return (L->Act ? 1 : 0);
}

/* Konec c206.c*/
