/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement par le principe "Least Recently Used"
 * 
 * \author Liavona Zheltanosava
 *
 * LRU_strategy.c,v 1.3 29/02/2016
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
 * la meme chose qu'un FIFO pour str_create, str_close et str_invalidate
 */
void *Strategy_Create(struct Cache *pcache) {
    return Cache_List_Create();
}

/*!
 * LRU : 
 */
void Strategy_Close(struct Cache *pcache) {
	Cache_List_Delete(C_LIST(pcache));
}

/*!
 * LRU : 
 */
void Strategy_Invalidate(struct Cache *pcache){
	Cache_List_Clear(C_LIST(pcache));
}

/*! 
 * LRU : en plus de FIFO on a les read et write
 * Ainsi le bloc en tête de la liste lru est-il le bloc le moins récemment utilisé, 
 * et c’est lui qui sera remplacé.
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
 * LRU
 * chaque fois qu’un bloc change d’affectation, on le transfère en queue
 * de liste
 * la meme chose pour strategy_write
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) {
    struct Cache_List *list = C_LIST(pcache); 
    Cache_List_Move_To_End(list, pbh);
}  

/*!
 * LRU : 
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh) {
    struct Cache_List *list = C_LIST(pcache);
    Cache_List_Move_To_End(list, pbh);
} 

char *Strategy_Name() {
    return "LRU";
}
