// Copyright 2022 Avram Cristian-Stefan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include "/home/student/utils.h"
#include "/home/student/struct.h"

#define MAX_LINE_SIZE 256
#define MAX_BOOK_NAME 40

void create_list(linked_list_t **list, unsigned int data_size) {
    *list = malloc(sizeof(linked_list_t));
    DIE(NULL == *list, "Malloc failed!");

    (*list)->data_size = data_size;
    (*list)->size = 0;
    (*list)->head = NULL;
}

void create_node(ll_node_t **node, unsigned int data_size,
                const void *new_data) {
    *node = malloc(sizeof(ll_node_t));
    DIE(NULL == *node, "Malloc failed!");

    (*node)->next = NULL;
    (*node)->data = malloc(data_size);
    (*node)->properties = NULL;
    memmove((*node)->data, new_data, data_size);
}

void ll_add_nth_node(linked_list_t* list, unsigned int n,
                    const void* new_data) {
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }
    create_node(&new_node, list->data_size, new_data);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

void ll_remove_nth_node(linked_list_t* list, unsigned int n) {
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return;
    }
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    info *data = (info *)curr->data;
    free(data->key);
    if (data->value != NULL)
        free(data->value);
    free(curr->data);
    free(curr);
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
    hashtable_t *hasht = malloc(sizeof(hashtable_t));
    DIE(NULL == hasht, "Malloc failed!");

    hasht->hmax = hmax;
    hasht->size = 0;
    hasht->hash_function = hash_function;
    hasht->compare_function = compare_function;

    hasht->buckets = malloc(hasht->hmax * sizeof(linked_list_t *));
    DIE(NULL == hasht->buckets, "Malloc failed!");

    for (unsigned int i = 0; i < hasht->hmax; i++)
        create_list(&hasht->buckets[i], sizeof(info));

    return hasht;
}

unsigned int hash_function_string(void *a) {
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

int compare_function_strings(void *a, void *b) {
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

int ht_has_key(hashtable_t *ht, void *key) {
    if (ht == NULL || key == NULL)
        return 0;

    unsigned int id = ht->hash_function(key) % ht->hmax;
    ll_node_t *current = ht->buckets[id]->head;
    while (current != NULL) {
        info *data = (info*)current->data;
        if (ht->compare_function(key, data->key) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}

void *ht_get(hashtable_t *ht, void *key) {
    if (ht == NULL || key == NULL)
        return NULL;

    if (ht_has_key(ht, key) == 0)
        return NULL;

    unsigned int id = ht->hash_function(key) % ht->hmax;
    ll_node_t *current = ht->buckets[id]->head;
    while (current != NULL) {
        info *data = (info*)current->data;
        if (ht->compare_function(key, data->key) == 0)
            return data->value;

        current = current->next;
    }

    return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
    unsigned int id = ht->hash_function(key) % ht->hmax;

    info data;
    data.key = malloc(key_size);
    DIE(NULL == data.key, "Malloc failed!");
    if (value != NULL) {
        data.value = malloc(value_size);
        DIE(NULL == data.value, "Malloc failed!");
    } else {
        data.value = NULL;
    }

    memmove(data.key, key, key_size);
    if (value != NULL)
        memmove(data.value, value, value_size);

    ll_add_nth_node(ht->buckets[id], ht->buckets[id]->size, &data);

    ht->size += 1;
}

void ht_remove_entry(hashtable_t *ht, void *key) {
    if (ht == NULL || key == NULL)
        return;

    if (ht_has_key(ht, key) == 0)
        return;

    unsigned int id = ht->hash_function(key) % ht->hmax;
    ll_node_t *node = ht->buckets[id]->head;
    info *data;
    int node_position = 0;

    while (node != NULL) {
        data = (info *)node->data;
        if (ht->compare_function(key, data->key) == 0)
            break;
        node_position++;
        node = node->next;
    }

    ll_remove_nth_node(ht->buckets[id], node_position);
    ht->size--;
}

void ht_free(hashtable_t *ht, int dict_created, int type)
{
    if (!dict_created)
        return;

    for (unsigned int i = 0; i < ht->hmax; i++) {
        if (ht->buckets[i]->head != NULL) {
            ll_node_t *node = ht->buckets[i]->head, *prev;

            while (node != NULL) {
                prev = node;
                node = node->next;

                info *data = (info *)prev->data;
                free(data->key);
                if (type == 1)
                    ht_free((hashtable_t *)data->value, 1, 2);
                if (type == 2) {
                    if (data->value != NULL)
                        free(data->value);
                }
                free(prev->data);
                free(prev->properties);
                free(prev);
            }
        }
        free(ht->buckets[i]);
    }

    free(ht->buckets);
    free(ht);
}

/*
* Function used to return the exact node / entry that the program
* needs. For example, if it is needed to acces some properties or datas,
* this function may be helpful as it is generic and works for any hashtable.
* It may seems that it looks a lot like GET() function and it does, but
* its job is to return a node and not a value.
*/
ll_node_t *return_key_node(hashtable_t *ht, void *key, unsigned int id) {
    ll_node_t *node = ht->buckets[id]->head;

    while (node->next != NULL) {
        info *datas = (info *)node->data;

        if (!ht->compare_function(datas->key, key))
            break;
        node = node->next;
    }

    return node;
}

/*
* Function used to remove a book completely, within its definitions.
* It may look like REMOVE() function, but there were much more things
* to write, as freeing the definitions hashtable and reinitialize the
* bucket if needed.
*/
void delete_book_node(hashtable_t *ht, void *key, unsigned int id) {
    ll_node_t *book_node = ht->buckets[id]->head, *prev;

    unsigned int pos = 0;
    while (book_node != NULL) {
        prev = book_node;
        book_node = book_node->next;

        info *data = (info *)prev->data;
        if (!ht->compare_function(data->key, key)) {
            ht_free((hashtable_t *)data->value, 1, 2);
            data->value = NULL;
            free(prev->properties);
            ll_remove_nth_node(ht->buckets[id], pos);
            break;
        }
        pos++;
    }
    if (!ht->buckets[id]->size) {
        free(ht->buckets[id]);
        create_list(&(ht->buckets[id]), sizeof(info));
        ht->size--;
    }
}

/*
* Function used to resize a hashtable.
* It actually creates a new hashtable with a doubled HMAX.
* The datas from the old hashtables are then stored in the new hashtable.
* There are 3 options:
* Option 1): definitions hashtable to be resized
*           - the definitions from every word / key will be copied
* Option 2): book hashtable to be resized
*           - every definitions hashtable from an entry will be
*           sent as an address to the new hashtable.
*           - the book nodes properties will be copied with memmove().
* Option 3): users hashtable to be resized
*           - every values, as days available for a borrowed book
*           or books' names will be sent as datas->value and then copied
*           as raw data.
*           - the user nodes properties will be copied with memmove()
*/
hashtable_t* resize_hasht(hashtable_t *ht, int option) {
    hashtable_t *new_ht = ht_create(ht->hmax * 2, hash_function_string,
                                    compare_function_strings);

    for (unsigned int i = 0; i < ht->hmax; i++) {
        if (ht->buckets[i]->head != NULL) {
            ll_node_t *node = ht->buckets[i]->head;

            while (node != NULL) {
                info *datas = (info *)node->data;
                unsigned int new_id = new_ht->hash_function(datas->key) %
                                    new_ht->hmax;
                char *tmp_key = (char *)datas->key;

                if (option == 1) {
                    char *def_name = (char *)datas->value;
                    ht_put(new_ht, datas->key, strlen(tmp_key) + 1,
                            datas->value, strlen(def_name) + 1);
                } else if (option == 2) {
                    hashtable_t *definitions = (hashtable_t *)datas->value;
                    ht_put(new_ht, datas->key, strlen(tmp_key) + 1,
                            definitions, sizeof(hashtable_t));
                    free(definitions);
                    ll_node_t *new_node = return_key_node(new_ht,
                                            datas->key, new_id);

                    if (!node->properties) {
                        node = node->next;
                        continue;
                    }
                    new_node->properties = malloc(2 * sizeof(int) +
                                            sizeof(int8_t));
                    DIE(NULL == new_node->properties, "Malloc failed!");

                    memmove(new_node->properties, node->properties,
                            2 * sizeof(int) + sizeof(int8_t));
                } else if (option == 3) {
                    int bytes = 0;
                    if (datas->value != NULL) {
                        char *borrowed_book = (char *)(datas->value +
                                            sizeof(int));
                        bytes = strlen(borrowed_book) + 1 + sizeof(int);
                    }

                    ht_put(new_ht, datas->key, strlen(tmp_key) + 1,
                            datas->value, bytes);
                    ll_node_t *new_node = return_key_node(new_ht,
                                        datas->key, new_id);

                    new_node->properties = malloc(2 * sizeof(int8_t) +
                                                sizeof(int));
                    DIE(NULL == new_node->properties, "Malloc failed!");

                    memmove(new_node->properties, node->properties,
                            2 * sizeof(int8_t) + sizeof(int));
                }
                node = node->next;
            }
        }
    }
    return new_ht;
}

// Function to read a book name within the ""
void read_dict_name(char book_name[MAX_BOOK_NAME]) {
    int len = 0;
    char *sp = " ";
    while (1) {
        char tmp[MAX_BOOK_NAME];
        scanf("%s", tmp);
        snprintf(book_name + len, MAX_BOOK_NAME, "%s", tmp);
        len += strlen(tmp);
        if (tmp[strlen(tmp) - 1] == '\"')
            break;
        snprintf(book_name + len, MAX_BOOK_NAME, "%s", sp);
        len += 1;
    }
}
