// Copyright 2022 Avram Cristian-Stefan

#ifndef _HOME_STUDENT_STRUCT_H_
#define _HOME_STUDENT_STRUCT_H_

typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
    void *properties;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

typedef struct info {
	void *key;
	void *value;
} info;

typedef struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
} hashtable_t;

#endif  //  _HOME_STUDENT_STRUCT_H_
