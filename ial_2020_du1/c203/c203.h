
/* ******************************* c203.h *********************************** */
/*  P�edm�t: Algoritmy (IAL) - FIT VUT v Brn�                                 */
/*  Hlavi�kov� soubor pro c203.c (Fronta znak� v poli)                        */
/*  �kol: c203 - Fronta znak� v poli                                          */
/*  Vytvo�il: V�clav Topinka, z��� 2005                                       */
/*  �pravy: Kamil Je��bek, z��� 2020                                          */
/* ************************************************************************** */

/* TENTO SOUBOR, PROS�ME, NEUPRAVUJTE! */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>

#define FALSE 0
#define MAX_QUEUE 50
extern int QUEUE_SIZE; 
/* 
 * Hodnota MAX_QUEUE ud�v� skute�nou velikost statick�ho pole pro ulo�en�
 * hodnot fronty. P�i implementaci operac� nad ADT fronta v�ak p�edpokl�dejte,
 * �e velikost tohoto pole je pouze QUEUE_SIZE. Umo�n� n�m to jednodu�e 
 * m�nit velikost fronty v pr�b�hu testov�n�. P�i implementaci ADT fronta byste
 * tedy hodnotu MAX_QUEUE nem�li v�bec pou��t. Pamatujte, �e do fronty se vejde
 * maxim�ln� (QUEUE_SIZE - 1) prvk�. Jedna pozice ve front� bude v�dy nevyu�it�,
 * aby bylo mo�n� odli�it pr�zdnou frontu od pln�. 
 */

extern int err_flag;                   /* Indikuje, zda operace volala chybu. */
extern int solved;                      /* Indikuje, zda byla operace �e�ena. */

                                        /* Chybov� k�dy pro funkci queueError */
#define MAX_QERR    5                                   /* po�et mo�n�ch chyb */
#define QERR_UP     1                                   /* chyba p�i stackTop */
#define QERR_FRONT  2                                   /* chyba p�i stackPop */
#define QERR_REMOVE 3                                  /* chyba p�i stackPush */
#define QERR_GET    4                                  /* chyba p�i stackPush */
#define QERR_INIT   5                                     /* chyba p�i malloc */

                               /* ADT fronta implementovan� ve statick�m poli */
typedef struct {
    char arr[MAX_QUEUE];                           /* pole pro ulo�en� hodnot */
    int f_index;                                       /* index prvn�ho prvku */
    int b_index;                                  /* index prvn� voln� pozice */
} tQueue;
                                      /* Hlavi�ky funkc� pro pr�ci s frontou. */
void queueError (int error_code);
void queueInit (tQueue* q);
int queueEmpty (const tQueue* q);
int queueFull (const tQueue* q);
void queueFront (const tQueue* q, char* c);
void queueRemove (tQueue* q);
void queueGet (tQueue* q, char* c);
void queueUp (tQueue* q, char c);

#endif

/* Konec hlavi�kov�ho souboru c203.h */
