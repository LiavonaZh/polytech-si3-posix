
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mymalloc.h"
#include "mymalloc_impl.h"

/*
    Fichier à compléter avec votre implémentation de mymalloc() et myfree().
*/

static struct Header base = {0, &base};	// celui qui pointe sur lui meme au debut
static struct Header *pfree = &base;  // pfree au depart est vide
static int free_blocks = 0; // cpt pour calculer le nb des blocks dans pfree

void *mymalloc(size_t sz) {

	int nh = 2 + (sz - 1) / HEADER_SZ; // formule d'enonce
	// printf("nheaders = %d\n", nheaders);
	struct Header *current;
	struct Header *previous;
	for (previous = pfree, current = pfree->next; current != pfree ; previous = current, current = current->next) {
		// si on a trouve un block qui correspond au memoire demande
		if (current->nheaders >= nh){
			// printf("dans grand if mymalloc\n");
			if (current->nheaders == nh) {
				// printf("dans if mymalloc\n");
				previous->next = current->next; // on vire ce block de pfree

				free_blocks--;
			}
			else {
				// printf("dans else if mymalloc\n");
				current->nheaders -= nh;
				current += current->nheaders; // on saute vers 'la fin' du block
				current->nheaders = nh;
			}
			return current + 1; // on renvoie le pointeur vers le premier adresse utile (saute header)
		}
		
	}

	// on alloue de memoire systeme minsystblock avec sbrk()
	if ((current = alloc_mega(nh)) == NULL) {
		return NULL;
	}

	return mymalloc(sz); // pfree est modifie si alloc_mega n'est pas null
}

void myfree(void *ptr) {
    struct Header *ptrh;
    struct Header *previous;
    struct Header *current;

    ptrh = (struct Header *)ptr - 1; // transformation; -1 pour decaler le truc
    
    for (previous = pfree, current = pfree->next;  ; previous = current, current = current->next) {
    	
    	// si on a fait un tour complet ou si current est le block a liberer
    	if (current == pfree || current > ptrh) {
    		break;
    	}
    }

    free_blocks++;
    // on fusionne avec le block libre qui est 'deriere' 
    if (ptrh + ptrh->nheaders == current) {
    	ptrh->nheaders += current->nheaders; // on somme les tailles des 2 blocks
        ptrh->next = current->next; // current est deplace sur ptrh

        free_blocks--;
    }

    else {
    	ptrh->next = current; // sinon on passe au block suivant
    }

    // meme truc pour le block precedent
    if (previous + previous->nheaders == ptrh) {
    	previous->nheaders += ptrh->nheaders;
    	previous->next = ptrh->next;

    	free_blocks--;
    }

    else {
    	previous->next = ptrh;
    }
}

void mymalloc_instrum() {
    printf("Nb des blocks libres dans pfree = %d\n", free_blocks);
    // a la fin d'execution free_blocks fait tjrs 1
}

void *alloc_mega(size_t sz) {
/*	struct Header * new;
	int nsysblock = 1 + (sz - 1)/MINSYSTBLOCK;
	int nhnew = nsysblock * MINSYSTBLOCK;
	size_t size_to_alloc = nhnew * HEADER_SZ;
	new = (struct Header *)sbrk(size_to_alloc);
	if ((intptr_t)new == -1) {
		return NULL;
	}
	new->nheaders = nhnew;
	new->next = NULL;
	myfree(new+1);
	return pfree;*/
// c'est la meme chose
// c'etatit fait pour debugger le truc de Benj Piat
	int nsysblock = (1 + (sz-1)/MINSYSTBLOCK)*MINSYSTBLOCK;
	void *yolo = sbrk(nsysblock*HEADER_SZ);
	if ((intptr_t) yolo == -1) {
		return NULL;
	}
	struct Header *new = (struct Header*) yolo;
	new->nheaders = nsysblock;
	new->next = NULL;
	myfree(new+1);
	return pfree;
}