#ifndef __DICT_H
#define __DICT_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef _TA_COMMON_H_
	#include <stdbool.h>
#endif

#define POWEROF2(x) ((((x)-1) & (x))==0)
#define MAX_UNSIGNED_32INT (0xFFFFFFFF)

typedef void(*hash_data_free_funct_t)(void *data);
typedef unsigned int(*hash_key_func_t)(const void *key,int klen);

/*
 * 哈希表节点 链式结构解决hash冲突
 */
typedef struct hash_node_st {

    
	void *val; //值
    int key_len;
    // 链往后继节点
    struct hash_node_st *next;
	unsigned int __hval; //当前节点的hash值，用于rehash
	char key[0]; //键 

} hash_node_st;

/*
 * 哈希表
 */
typedef struct hash {

    // 哈希表节点指针数组（俗称桶，bucket）
    hash_node_st **slots;
	
	//初始化hash节点数量
	unsigned int nslot;

	// hash表中允许最大的节点数量(超过此数量，将会进行rehash 扩容)
	unsigned int max_element;

	// hash表中允许最小的节点数量(小于此数量，将会进行rehash 收缩)
	unsigned int min_element;

    // 哈希表现有的节点数量
    unsigned long nelement;

	//自定义的hash释放函数
	hash_data_free_funct_t hdel;

	//自定义的hash生成函数
	hash_key_func_t hkey;
	
} hash_st;



/*
	创建hash
*/
hash_st * hash_create(hash_data_free_funct_t del,hash_key_func_t keyf,
							unsigned int slotnum);
/*
	hash添加
*/
int hash_insert(hash_st* ht, const void *key,int len,void *val);

/*
	hash查找
*/
int hash_search(hash_st* ht, const void *key,int len,void **val);

/*
	hash删除
*/
int hash_delete(hash_st* ht, const void *key,int len);


/*
	hash释放
*/
void hash_destory(hash_st *ht);




/**********************************************************************************/

#endif /* __DICT_H */


