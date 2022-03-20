#include "hash.h"

#ifndef _TA_COMMON_H_
	#include<stdio.h>
	#define err_log printf 
	#define info_log printf
	#define dbg_log printf
#endif

static uint32_t dict_hash_function_seed = 5381;

void 
free_hash(dictEntry * entry)
{
	free(entry->key);
	free(entry);
}
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
 * Description   : 初始化hash
 * Input         : struct hash * dict  
                int hash_node       
 * Output        : None
 * Return        : struct
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
struct hash* hash_init(struct hash * dict,int hash_node){
	if(hash_node < 0 || !dict){
		return NULL;
	}
	dict->table = (dictEntry **)malloc(sizeof(dictEntry*)*hash_node);
	if(dict->table == NULL){
		return NULL;
	}
	memset(dict->table, 0, sizeof(dictEntry *)*hash_node);
	dict->size = hash_node;
	dict->sizemask = hash_node -1;
	dict->used = 0;
	return dict;
}
/*****************************************************************************
 * Function      : dup_string
 * Description   : 复制长度为len的字符串val，并返回指针，内存由调用者释放
 * Input         : const char * val  
                int len           
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
void *dup_string(const char * val,int len){
	if(val == NULL || len< 0){
		return NULL;
	}
	char * tmp = (char*)malloc(sizeof(char)*(len+1));
	if (tmp == NULL){
		return NULL;
	}
	memcpy(tmp,val,len);
	tmp[len] = '\0';
	return tmp;
	
}
/*****************************************************************************
 * Function      : hash_add
 * Description   : hash节点添加（TODO 支持自动扩容）
 * Input         : struct hash *dict  
                const char * key   
                void * val         
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
int hash_add(struct hash *dict,const char * key ,void * val ){
	if(!dict || !key){
		return -1;
	}
	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key, key_len)%dict->sizemask;
	struct dictEntry * entry = (struct dictEntry *)malloc(sizeof(struct dictEntry));
	if(entry == NULL){
		return -1;
	}
	entry->key = dup_string(key,key_len);
	entry->key_len = key_len;
	entry->val = val;
	entry->next = dict->table[key_index];//前插
	dict->table[key_index] = entry;
	dict->used++;
}
/*****************************************************************************
 * Function      : hash_delete
 * Description   : 删除指定key的节点
 * Input         : struct hash *dict  
                const char * key   
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
void* hash_delete(struct hash *dict,const char * key){
	if (!dict || !key) {
		return NULL;
	}

	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key,key_len)%dict->sizemask;
	struct dictEntry* entry = dict->table[key_index];
	struct dictEntry* pre_entry = NULL;
	while(entry != NULL){
		if(entry->key_len == key_len &&
			!strncmp(entry->key,key,key_len)){
			if( pre_entry == NULL){
				dict->table[key_index] = entry->next;
			} else {
				pre_entry->next = entry->next;
			}
			void * tmp_ptr = entry->val;
			FREE_ENTRY(entry);
			dict->used--;
			return tmp_ptr;
		}else{
			
			pre_entry = entry;	//存在hash冲突，当前节点不是要删除节点，先记录前置节点
			entry = entry->next; //移动到下一个节点
		}
	}
	return NULL;
}
/*****************************************************************************
 * Function      : hash_find
 * Description   : 根据key查找对应hash，并返回entry,找不到返回NULL
 * Input         : struct hash *dict  
                char *key          
 * Output        : None
 * Return        : struct
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
struct dictEntry* hash_find(struct hash *dict,char *key){
	if (!dict || !key) {
		return NULL;
	}
	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key,key_len)%dict->sizemask;
	struct dictEntry* entry = dict->table[key_index];
	while (entry){
		if(entry->key_len == key_len && 
			!strncmp(entry->key,key,key_len)){
			return entry;
		}else{
			entry = entry->next;
		}
	}
	return NULL;
}

/*****************************************************************************
 * Function      : hash_destory_whit_func
 * Description   : 释放hash
 * Input         : struct hash* dict          
                void (* freefunc)(void *)  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20220320
 *   Author      : lxc
 *   Modification: Created function

*****************************************************************************/
void hash_destory_whit_func(struct hash* dict,void (* freefunc)(void *)){
	dictEntry *dc,*next;
	if (dict == NULL){
		return ;
	}
	if (freefunc == NULL){
		freefunc = &free;
	}
	HASH_FOR_EACH_SAFE(dc, next, dict){
		freefunc(dc->val);
		FREE_ENTRY(dc);
	}
	if(dict->table){
		free(dict->table);
	}
}
struct dictEntry *hash_next(struct hash * hash, struct dictEntry * entry){
	if(entry == NULL || hash == NULL){
		return NULL;
	}

	if( entry->next){
		return entry->next;
	} else {
		int key = hashGenHashFunction(entry->key,strlen(entry->key))%hash->sizemask + 1;
		if( key >= hash->sizemask){
			return NULL;
		} else {
			for( ; key < hash->sizemask ; key++){
				if( hash->table[key] != NULL){
					return hash->table[key];
				}
			}
		}
	}
	return NULL;
}

struct dictEntry *hash_first(struct hash * hash){
	if(hash == NULL || hash->used == 0){
		return NULL;
	}

	int i = 0;
	for( i = 0 ; i <= hash->sizemask; i++){
		if(hash->table[i] != NULL){
			return hash->table[i];
		}
	}
	return NULL;
}

