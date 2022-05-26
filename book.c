// Copyright 2022 Avram Cristian-Stefan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "/home/student/book.h"
#include "/home/student/struct.h"
#include "/home/student//utils.h"

/*
* Function used to change the definition of a word / key if
* there is added a definition with the keyword actually stored
* in the hashtable.
*/
void change_def(hashtable_t *definitions, char *key, char *val) {
    unsigned int id = definitions->hash_function(key) % definitions->hmax;

    ll_node_t *node = return_key_node(definitions, key, id);

    info *datas = (info *)node->data;
    free(datas->value);
    datas->value = malloc(strlen(val) + 1);
    DIE(NULL == datas->value, "Malloc failed!");

    memmove(datas->value, val, strlen(val) + 1);
}

/*
* Function to initialize the properties of a book node.
* The datas are stored in the following order:
* book_node->properties ==> | IS_BORROWED | PURCHASES | RATING |s
* These properties are stored in a generic array, so there must be used
* pointers arithmetic.
*/
void initialize_book_data(hashtable_t **library, char *name) {
    unsigned int id = (*library)->hash_function(name) % (*library)->hmax;
    ll_node_t *node = return_key_node(*library, name, id);

    if (node->properties)
        free(node->properties);
    node->properties = malloc(2 * sizeof(int) + sizeof(int8_t));
    DIE(NULL == node->properties, "Malloc failed!");

    int8_t is_borrowed = 0;
    int purchases = 0;
    int rating = 0;

    memmove(node->properties, &is_borrowed, sizeof(int8_t));
    memmove(node->properties + sizeof(int8_t), &purchases, sizeof(int));
    memmove(node->properties + sizeof(int8_t) + sizeof(int), &rating,
            sizeof(int));
}

/*
* Function to add a book in the hashtable.
* It presents resizing options for the library hashtable
* and definitions hashtable.
* When adding the entry in the book hashtable, the value
* will be represented by the created definitions hashtable.
*/
void add_book(hashtable_t **library, int *library_created) {
    char name[MAX_BOOK_NAME];
    int nr_of_def;

    read_dict_name(name);
    scanf("%d", &nr_of_def);

    if (!(*library_created)) {
        *library = ht_create(HMAX, hash_function_string,
				compare_function_strings);
        *library_created = 1;
    }

    hashtable_t *definitions = ht_create(HMAX, hash_function_string,
                            compare_function_strings);

    if (ht_has_key(*library, name)) {
        unsigned int book_id = (*library)->hash_function(name) %
                                (*library)->hmax;
        delete_book_node(*library, name, book_id);
    }

    for (int i = 0; i < nr_of_def; i++) {
        char key[MAX_KEY_VAL_USER_SIZE];
        char val[MAX_KEY_VAL_USER_SIZE];
        scanf("%s %s", key, val);

        if (ht_has_key(definitions, key)) {
            change_def(definitions, key, val);
            continue;
        }
        ht_put(definitions, key, strlen(key) + 1, val, strlen(val) + 1);

        // Resize definitions hashtable
        if (definitions->size > definitions->hmax) {
            hashtable_t *new_ht = resize_hasht(definitions, 1);
            ht_free(definitions, 1, 2);
            definitions = new_ht;
        }
    }

    ht_put(*library, name, strlen(name) + 1, definitions, sizeof(hashtable_t));
    initialize_book_data(library, name);

    if ((*library)->size > (*library)->hmax) {
        hashtable_t *new_ht = resize_hasht(*library, 2);
        ht_free(*library, 1, 3);
        *library = new_ht;
    }

    free(definitions);
}

/*
* This function is used for two commands:
* Option 1): Prints the value for a given key / definition.
* Option 2): Removes the given key / definition from a book.
*/
void get_or_remove_def(hashtable_t *library, int option) {
    char name[MAX_BOOK_NAME];
    char def[MAX_KEY_VAL_USER_SIZE];

    read_dict_name(name);
    scanf("%s", def);

    if (ht_has_key(library, name)) {
        hashtable_t *definitions = (hashtable_t *)ht_get(library, name);

        if (ht_has_key(definitions, def)) {
            if (option == 1) {
                printf("%s\n", (char *)ht_get(definitions, def));
            } else if (option == 2) {
                ht_remove_entry(definitions, def);
            }
        } else {
            printf("The definition is not in the book.\n");
        }
    } else {
        printf("The book is not in the library.\n");
    }
}

/*
* Function that is used to add a definition into a book.
* It presents the resizing case and changes the data->value pointer
* in case of resizing.
*/
void add_def(hashtable_t *library) {
    char name[MAX_BOOK_NAME];
    char key[MAX_KEY_VAL_USER_SIZE];
    char val[MAX_KEY_VAL_USER_SIZE];

    read_dict_name(name);
    scanf("%s %s", key, val);

    if (ht_has_key(library, name)) {
        unsigned int id = library->hash_function(name) % library->hmax;
        ll_node_t *node = return_key_node(library, name, id);

        info * datas = (info *)node->data;
        hashtable_t *definitions = (hashtable_t *)datas->value;

        if (ht_has_key(definitions, key)) {
            change_def(definitions, key, val);
        } else {
            ht_put(definitions, key, strlen(key) + 1, val, strlen(val) + 1);
            if (definitions->size > definitions->hmax) {
                hashtable_t *new_ht = resize_hasht(definitions, 1);
                ht_free(definitions, 1, 2);
                definitions = new_ht;
            }
            datas->value = definitions;
        }
    } else {
        printf("The book is not in the library.\n");
    }
}

/*
* Function to entirely remove a book from the library hashtable.
* It deletes and frees all of the datas from this book, inclusively the
* definitions hashtable.
*/
void remove_book(hashtable_t **library) {
    char name[MAX_BOOK_NAME];
    read_dict_name(name);

    if (!ht_has_key(*library, name)) {
        printf("The book is not in the library.\n");
        return;
    }

    unsigned int id = (*library)->hash_function(name) % (*library)->hmax;
    delete_book_node(*library, name, id);
}

/*
* Function that is used to print the number of purchases
* and the rating that are stored inside a book_node->properites.
*/
void get_book(hashtable_t *library) {
    char name[MAX_BOOK_NAME];
    read_dict_name(name);

    if (!ht_has_key(library, name)) {
        printf("The book is not in the library.\n");
        return;
    }

    // Formatted name refers to the dictionary names and it
    // eliminates the "" from inside this name.
    char formatted_name[MAX_BOOK_NAME];
    memcpy(formatted_name, name, strlen(name) + 1);
    formatted_name[strlen(formatted_name) - 1] = '\0';
    printf("Name:%s", formatted_name + 1);

    unsigned int id = library->hash_function(name) % library->hmax;
    ll_node_t *node = return_key_node(library, name, id);

    int purchases = *(int *)(node->properties + sizeof(int8_t));
    int rating = *(int *)(node->properties + sizeof(int8_t) + sizeof(int));

    float float_rating;

    if (!purchases) {
        float_rating = 0.0;
    } else {
        float_rating = (float)rating / (float)purchases;
    }

    printf(" Rating:%.3f", float_rating);
    printf(" Purchases:%d\n", purchases);
}
