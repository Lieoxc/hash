#include<stdio.h>
#include"hash.h"

int main(void) {
	struct hash* my_hash_test;
	char * key ="lxc";
	hash_init(my_hash_test, 5);
	hash_add(my_hash_test, key, "good");
	struct dictEntry* find_entry = hash_find(my_hash_test, key);
	if (find_entry != NULL){
		printf("heelo owrld:%s:%s\n",key,(char*)find_entry->val);
	}
	hash_delete(my_hash_test, key);
	find_entry = hash_find(my_hash_test, key);
	if (find_entry == NULL){
		printf("can not find :%s data\n",key);
	}	
}

