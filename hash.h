#ifndef __DICT_H
#define __DICT_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef _TA_COMMON_H_
	#include <stdbool.h>
#endif



/*
 * 操作返回状态
 */
#define DICT_OK 0
#define DICT_ERR 1
#define DEFAULT_HASH_NODE 20
#define FREE_ENTRY(entry) free_hash(entry)

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)


#define HASH_FOR_EACH(dict,hash) \
	for((dict = hash_first(hash)) ; dict != NULL ; dict = hash_next(hash,dict)) 

/*******************************************************************************
*@ Description    :   当遍历过程中需要对hash进行操作时，比如删除，可以使用以下的宏
*@ Input          :   当dict
*@ Output         :
*@ Return         :
*******************************************************************************/

#define HASH_FOR_EACH_SAFE(dict, next, hash) \
	for((dict = hash_first(hash)) ;((next = hash_next(hash,dict)) && 0) || dict != NULL ; dict = next) 

/*
 * 哈希表节点 链式结构解决hash冲突
 */
typedef struct dictEntry {

    // 键
    void *key;
	void *val;
    int key_len;
    // 链往后继节点
    struct dictEntry *next;
	unsigned int __hval; //当前节点的hash值，用于rehash

} dictEntry;

/*
 * 哈希表
 */
typedef struct hash {

    // 哈希表节点指针数组（俗称桶，bucket）
    dictEntry **table;      

    // 指针数组的大小
    unsigned long size;     

    // 指针数组的长度掩码，用于计算索引值
    unsigned long sizemask; 

    // 哈希表现有的节点数量
    unsigned long used;     //没有使用   

} hash;

/*
	计算hash key
*/
unsigned int hashGenHashFunction(const void *key, int len);

/*
	初始化hash
*/
struct hash * hash_init(struct hash * dict , int hash_node);

/*
	添加hash节点
*/
int hash_add(struct hash *dict,const char * key ,void * val );

/*
	删除hash节点
*/
void* hash_delete(struct hash *dict,const char * key);

/*
	删除hash节点
*/
struct dictEntry* hash_find(struct hash *dict,char *key);


/*
	释放hash
*/
void hash_destory_whit_func(struct hash* dict,void (* freefunc)(void *));

struct dictEntry *hash_next(struct hash * hash, struct dictEntry * entry);

struct dictEntry *hash_first(struct hash * hash);


/**********************************************************************************/

#endif /* __DICT_H */


