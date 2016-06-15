/*!
 * \file FIFO_strategy.c
 *
 * \brief  Stratégie de remplacement par le principe FIFO
 * 
 * \author Liavona Zheltanosava
 *
 * FIFO_strategy.c,v 1.3 29/02/2016
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"

#include "cache_list.h"

// cast la cache en une liste des blocs
#define C_LIST(pcache) ((struct Cache_List *)((pcache)->pstrategy))

/*!
 * 'une liste de pointeurs sur les blocs valides du cache'
 */
void *Strategy_Create(struct Cache *pcache) {
    return Cache_List_Create();
}

/*!
 * FIFO : 
 */
void Strategy_Close(struct Cache *pcache) {
	Cache_List_Delete(C_LIST(pcache));
}

/*!
 * FIFO : 
 */
void Strategy_Invalidate(struct Cache *pcache){
	Cache_List_Clear(C_LIST(pcache));
}

/*! 
 * FIFO : On prend le premier bloc invalide. S'il n'y en a plus, on prend un bloc au hasard.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) {

    struct Cache_Block_Header *pbh;
    struct Cache_List *c_list = C_LIST(pcache);

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) {
        Cache_List_Append(c_list, pbh);
        return pbh;
    } 
    else {
	    // On prend le premier de la liste (le plus ancien)
	    pbh = Cache_List_Remove_First(c_list);

	    // on le met a la fin de la list
	    Cache_List_Append(c_list, pbh);
	}
    return pbh;   
}


/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) {
}  

/*!
 * FIFO : Rien à faire ici.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh) {
} 

char *Strategy_Name() {
    return "FIFO";
}
