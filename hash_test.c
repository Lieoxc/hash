#include<stdio.h>
#include"hash.h"

int main(void) {
	hash_st* my_hash_test;
	char * key ="lxc";
	my_hash_test = hash_create(NULL,NULL,8);
	hash_insert(my_hash_test, key,strlen(key), "good");
	char *hash_data = NULL;
	if (hash_search(my_hash_test, key,strlen(key),(void**)&hash_data) == 0){
		printf("heelo owrld:%s:%s\n",key,hash_data);
	}
	hash_delete(my_hash_test, key,strlen(key));

	hash_data=NULL;
	if (hash_search(my_hash_test, key,strlen(key),(void**)&hash_data) != 0){
		printf("can not find :%s data\n",key);
	}	
}

