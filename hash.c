#include "hash.h"

#ifndef _TA_COMMON_H_
	#include<stdio.h>
	#define err_log printf 
	#define info_log printf
	#define dbg_log printf
#endif

static uint32_t dict_hash_function_seed = 5381;


/*****************************************************************************
 * Function      : hashGenHashFunction
 * Description   : 计算hash key
 * Input         : const void *key  
                int len          
 * Output        : None
 * Return        : unsigned
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function
*****************************************************************************/
unsigned int hashGenHashFunction(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
 	h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}
/*****************************************************************************
 * Function      : hash_create
 * Description   : 创建一个hash表
 * Input         : hash_data_free_funct_t del  
                hash_key_func_t keyf        
                unsigned int slotnum        
 * Output        : None
 * Return        : hash_st
 * Others        : 
 * Record
 * 1.Date        : 20220321
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
hash_st * hash_create(hash_data_free_funct_t del,hash_key_func_t keyf,
							unsigned int slotnum)
{
	hash_st *h;
	//只支持创建2的平方数量
	if(!POWEROF2(slotnum)){
		return NULL;
	}
	h = malloc(sizeof(hash_st));
	if(h == NULL){
		return NULL;
	}
	if(slotnum == 0){ /* 自动增长 */
		h->nslot = 1<<3;
		h->max_element = 1<<5;
		h->min_element = 0;
	}else{
		h->nslot = slotnum;
		h->max_element = MAX_UNSIGNED_32INT;
		h->min_element = 0;
	}
	h->slots = malloc(h->nslot*sizeof(struct hash_node_st *));
	if(h->slots == NULL){
		free(h);
		return NULL;
	}
	h->hdel = del;
	if(keyf){
		h->hkey =keyf;
	}else{
		h->hkey = hashGenHashFunction;
	}
	return h;
}

/*****************************************************************************
 * Function      : hash_rehash
 * Description   : hash扩容收缩:flag小于0表示收缩，flag大于0表
 					示扩张，等于0什么都不干
 * Input         : hash_st *h  
                int flag    
 * Output        : None
 * Return        : static
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
static void hash_rehash(hash_st *h,int flag){

}
/*****************************************************************************
 * Function      : hash_insert
 * Description   : 插入一个键值对到hash
 * Input         : hash_st* ht      
                const void *key  
                int len          
                void *val        
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220321
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
int hash_insert(hash_st* ht, const void *key,int len,void *val){
	if( !ht || !key){
		return -1;
	}
	unsigned int hval = ht->hkey(key,len);//计算出hash值
	unsigned int idx = hval & (ht->nslot -1); //获取table数组的索引(数组下标)
	struct hash_node_st *tmp;
	struct hash_node_st *p = ht->slots[idx];
		 
	while(p){
		if(hval == p->__hval
			&& p->key_len == len && memcmp(p->key,key,len) == 0){
				ht->hdel(p->val);
				p->val = val;
				return 0;
			}
			p = p->next;
	}

	//新节点
	tmp = malloc(sizeof(struct hash_node_st) + len);

	if(!tmp){
		return -1;
	}

	memset(tmp,0,sizeof(struct hash_node_st) + len);
	tmp->key_len = len;
	memcpy(tmp->key,key,len);
	
	tmp->val = val;
	tmp->__hval = hval;
	tmp->next = ht->slots[idx];
	ht->slots[idx] =tmp;
	ht->nelement++;
	
	if(ht->nelement > ht->max_element){
		hash_rehash(ht, 1);
	}
	return 0;
}
/*****************************************************************************
 * Function      : hash_search
 * Description   : 在hash中查找键值对
 * Input         : hash_st* ht      
                const void *key  
                int len          
                void **val       
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220321
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
int hash_search(hash_st* ht, const void *key,int len,void **val){
	if( !ht || !key){
		return -1;
	}
	unsigned int hval = ht->hkey(key,len);//计算出hash值
	unsigned int idx = hval & (ht->nslot -1); //获取table数组的索引(数组下标)
	struct hash_node_st *p = ht->slots[idx];
	while(p){
		if(hval == p->__hval
			&& p->key_len == len && memcmp(p->key,key,len) == 0){
				if(val){
					*val = p->val;
				}
				return 0;
			}
			p = p->next;
	}
	return -1;
}
/*****************************************************************************
 * Function      : hash_delete
 * Description   : 删除一个键值对
 * Input         : hash_st* ht      
                const void *key  
                int len          
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220321
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
int hash_delete(hash_st* ht, const void *key,int len){
	if( !ht || !key){
		return -1;
	}
	unsigned int hval = ht->hkey(key,len);//计算出hash值
	unsigned int idx = hval & (ht->nslot -1); //获取table数组的索引(数组下标)
	struct hash_node_st *p = ht->slots[idx];
	struct hash_node_st *pre_entry = NULL;

	while(p){
		if(hval == p->__hval
			&& p->key_len == len && memcmp(p->key,key,len) == 0){
			if(pre_entry){
				pre_entry->next = p->next;
			}else{
				ht->slots[idx] = p->next;
			}
			ht->nelement--;
			if((void *)ht->hdel)
				ht->hdel(p->val);
			free(p);
			if(ht->nelement > ht->min_element){
				hash_rehash(ht, -1);
			}
			return 0;
		}
		pre_entry = p;
		p = p->next;
	}
}
/*****************************************************************************
 * Function      : hash_destory
 * Description   : hash释放
 * Input         : hash_st *ht  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220321
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
void hash_destory(hash_st *ht){
	unsigned int i;
	struct hash_node_st *t;
	for(i = 0; i< ht->nslot;i++){
		while(ht->slots[i]){
			t = ht->slots[i];
			if((void*)ht->hdel){
				ht->hdel(t->val);
			}
			free(t);
			ht->slots[i] = ht->slots[i]->next;
		}
	}
	free(ht->slots);
	free(ht);
}

