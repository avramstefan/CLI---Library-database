// Copyright 2022 Avram Cristian-Stefan

#ifndef _HOME_STUDENT_UTILS_H_
#define _HOME_STUDENT_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "/home/student/struct.h"

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)
#define MAX_COMMAND_SIZE 30
#define MAX_LINE_SIZE 256
#define HMAX 10
#define MAX_BOOK_NAME 40
#define MAX_KEY_VAL_USER_SIZE 20

void create_list(linked_list_t **list, unsigned int data_size);
void create_node(ll_node_t **node, unsigned int data_size,
                const void *new_data);
void ll_add_nth_node(linked_list_t* list, unsigned int n,
                    const void* new_data);
void ll_remove_nth_node(linked_list_t* list, unsigned int n);
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));
unsigned int hash_function_string(void *a);
int compare_function_strings(void *a, void *b);
int ht_has_key(hashtable_t *ht, void *key);
void *ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht, int dict_created, int type);
hashtable_t* resize_hasht(hashtable_t *ht, int option);
ll_node_t *return_key_node(hashtable_t *ht, void *key, unsigned int id);
void read_dict_name(char book_name[MAX_LINE_SIZE]);
void delete_book_node(hashtable_t *ht, void *key, unsigned int id);

#endif  //  _HOME_STUDENT_UTILS_H_
