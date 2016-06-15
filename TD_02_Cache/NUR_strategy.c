/*!
 * \file NUR_strategy.c
 *
 * \brief  Stratégie de remplacement par le principe "Not Used Recently"
 * 
 * \author Liavona Zheltanosava
 *
 * NUR_strategy.c,v 1.3 07/03/2016
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"

#include "cache_list.h"

/*!
* Initialise le compteur et met le flag REFER à 0 pour tous les blocs du cache
*/
void initialize_flag_R(struct Cache *pcache) {
    // Si la limite > 0
    if (pcache->nderef > 0)
    {
        // On met le flag REFER à 0 à tous les blocs
        for (int i = 0; i < pcache->nblocks; ++i)
            pcache->headers[i].flags &= ~REFER;


        // Ré initilitialise le compteur
        pcache->pstrategy = 0;

        // Incrémentation du compteur de déréférencement
        ++pcache->instrument.n_deref;
    }
}

/*!
* Calcul la valeur RM (RM = 2*R+M) du bloc en paramètre
*/
int calculate_RM(struct Cache_Block_Header *bloc) {
    int rm = 0;
    if ((bloc->flags & REFER) > 0) rm += 2;
    if ((bloc->flags & MODIF) > 0) rm += 1;

    return rm;
}

/*!
 * 'une liste de pointeurs sur les blocs valides du cache'
 * la meme chose qu'un FIFO pour str_create, str_close et str_invalidate
 */
void *Strategy_Create(struct Cache *pcache) {
    return pcache->pstrategy = (int*)0;
}

/*!
 * NUR : 
 */
void Strategy_Close(struct Cache *pcache) {
	
}

/*!
 * NUR : reinitialisation 
 */
void Strategy_Invalidate(struct Cache *pcache){
	initialize_flag_R(pcache);
}

/*! 
 * NUR : Retourne le premier bloc free ou celui qui a le plus petit RM
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) {

    struct Cache_Block_Header *pbh;
    int min_rm = 0;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) {
        return pbh;
    } 
    else {
        // Pas de bloc libre, on cherche le bloc NUR
        for (int i = 0; i < pcache->nblocks; i++){
            struct Cache_Block_Header *current = &pcache->headers[i];
            int rm = calculate_RM(current);
            if (rm == 0)
                return current;
            else {
                if (min_rm == (int) NULL || rm < min_rm){
                    min_rm = rm;
                    pbh = current;
                }
            }
        }
    }
    return pbh;  
}


/*!
 * NUR : chaque fois que le bloc a ete reference, on met R a 1
 * donc pareil pour ecriture et lecture
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) {
    // Si le compteur est >= à la limit, on ré initialize
    if ( (int)++pcache->pstrategy >= pcache->nderef)
        initialize_flag_R(pcache);

    // On met le flag REFER à 1
    pbh->flags |= REFER;
}  

/*!
 * NUR : pareil qu'en read
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh) {
    // Si le compteur est >= à la limit, on ré initialize
    if ( (int)++pcache->pstrategy >= pcache->nderef)
        initialize_flag_R(pcache);

    // On met le flag REFER à 1
    pbh->flags |= REFER;
} 

char *Strategy_Name() {
    return "NUR";
}
