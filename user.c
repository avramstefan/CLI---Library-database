// Copyright 2022 Avram Cristian-Stefan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "/home/student/user.h"
#include "/home/student/exit.h"
#include "/home/student/struct.h"
#include "/home/student/utils.h"

/*
* Function to initialize the properties of a user node.
* The datas are stored in the following order:
* user_node->properties ==> | PERMISSION | BORROWED_A_BOOK | POINTS |
* These properties are stored in a generic array, so there must be used
* pointers arithmetic.
*/
void initialize_user_data(hashtable_t **users, char *name) {
    unsigned int id = (*users)->hash_function(name) % (*users)->hmax;
    ll_node_t *node = return_key_node(*users, name, id);

    if (node->properties)
        free(node->properties);
    node->properties = malloc(2 * sizeof(int) + sizeof(int8_t));
    DIE(NULL == node->properties, "Malloc failed!");

    int8_t permission = 1;
    int8_t borrowed_book = 0;
    int points = 100;

    memmove(node->properties, &permission, sizeof(int8_t));
    memmove(node->properties + sizeof(int8_t), &borrowed_book, sizeof(int8_t));
    memmove(node->properties + 2 * sizeof(int8_t), &points, sizeof(int));

    info *datas = (info *)node->data;
    datas->value = NULL;
}

/*
* Function to add a user in the users hashtable.
* It treats the resizing case and initialize datas.
* When adding to the hashtable, as a value, it will be
* sent NULL, as its worth may improve with the number of
* books that he's reading :).
*/
void add_user(hashtable_t **users, int *user_created) {
    char name[MAX_KEY_VAL_USER_SIZE];
    scanf("%s", name);

    if (!(*user_created)) {
        *users = ht_create(HMAX, hash_function_string,
				compare_function_strings);
        *user_created = 1;
    }

    if (ht_has_key(*users, name)) {
        printf("User is already registered.\n");
        return;
    }

    ht_put(*users, name, strlen(name) + 1, NULL, 0);
    initialize_user_data(users, name);

    // Resizing the users hashtable in case of need
    if ((*users)->size > (*users)->hmax) {
        hashtable_t *new_ht = resize_hasht(*users, 3);
        ht_free(*users, 1, 2);
        *users = new_ht;
    }
}
/*
* A function that is called whenever a user wants to borrow a book.
* If a user may have the chance to do that and there are no stop signs,
* the value of his hash key will be storing the name of the book and the
* number of days that it'll be available as in the following order:
* user_node->datas->value ==> | DAYS AVAILABLE | NAME OF THE BOOK |
*/
void borrow(hashtable_t *users, hashtable_t *library, int user_created,
            int library_created) {
    if (!user_created) {
        printf("You are not registered yet.\n");
        return;
    }

    if (!library_created) {
        printf("The book is not in the library.\n");
        return;
    }

    char user_name[MAX_KEY_VAL_USER_SIZE];
    char book_name[MAX_BOOK_NAME];
    int days_available;

    scanf("%s", user_name);
    read_dict_name(book_name);
    scanf("%d", &days_available);

    if (!ht_has_key(users, user_name)) {
        printf("You are not registered yet.\n");
        return;
    }

    unsigned int user_id = users->hash_function(user_name) % users->hmax;
    unsigned int book_id = library->hash_function(book_name) % library->hmax;

    ll_node_t *user_node = return_key_node(users, user_name, user_id);

    // Verify permission.
    if (!(*(int8_t *)(user_node->properties))) {
        printf("You are banned from this library.\n");
        return;
    }

    // Verify if there is already a book that the user has borrowed.
    if (*(int8_t *)(user_node->properties + sizeof(int8_t))) {
        printf("You have already borrowed a book.\n");
        return;
    }

    // Verify if the given book is in the library.
    if (!ht_has_key(library, book_name)) {
        printf("The book is not in the library.\n");
        return;
    }

    ll_node_t *book_node = return_key_node(library, book_name, book_id);

    // Verify if the book is available.
    if (*(int8_t *)(book_node->properties)) {
        printf("The book is borrowed.\n");
        return;
    }

    info *user_datas = user_node->data;
    if (user_datas->value == NULL)
        user_datas->value = malloc(sizeof(int) + strlen(book_name) + 1);
    else
        user_datas->value = realloc(user_datas->value, sizeof(int) +
                                    strlen(book_name) + 1);
    DIE(NULL == user_datas->value, "Failed to allocate/reallocate!");

    memmove(user_datas->value, &days_available, sizeof(int));
    memmove(user_datas->value + sizeof(int), book_name, strlen(book_name) + 1);

    int8_t borrowed_book = 1;
    memmove(user_node->properties + sizeof(int8_t), &borrowed_book,
            sizeof(int8_t));

    memmove(book_node->properties, &borrowed_book, sizeof(int8_t));
}

/*
* Function used to return a book.
* It verifies the correspondence between the passed days
* and the days that the borrowed book was available.
*/
void return_book(hashtable_t *users, hashtable_t *library) {
    char user_name[MAX_KEY_VAL_USER_SIZE];
    char book_name[MAX_BOOK_NAME];
    int passed_days, rating;

    scanf("%s", user_name);
    read_dict_name(book_name);
    scanf("%d %d", &passed_days, &rating);

    unsigned int user_id = users->hash_function(user_name) % users->hmax;
    unsigned int book_id = library->hash_function(book_name) % library->hmax;
    ll_node_t *user_node = return_key_node(users, user_name, user_id);
    ll_node_t *book_node = return_key_node(library, book_name, book_id);

    // Verify permission.
    if (!(*(int8_t *)(user_node->properties))) {
        printf("You are banned from this library.\n");
        return;
    }

    // Verify if the user has borrowed any books.
    if (!(*(int8_t *)(user_node->properties + sizeof(int8_t)))) {
        printf("You didn't borrow this book.\n");
        return;
    }

    info *user_datas = (info *)user_node->data;
    int days_available = *(int *)(user_datas->value);
    int actual_points = *(int *)(user_node->properties + 2 * sizeof(int8_t));

    // Verify if the borrowed book is the same with the given book.
    char *borrowed_book = (char *)(user_datas->value + sizeof(int));
    if (strcmp(borrowed_book, book_name)) {
        printf("You didn't borrow this book.\n");
        return;
    }

    // Calculates the number of points to be added or substracted
    if (days_available > passed_days) {
        actual_points += (days_available - passed_days);
    } else if (passed_days > days_available) {
        actual_points -= (2 * (passed_days - days_available));
    }

    // Marks to book->properties that it is borrowed and adds the rating
    int8_t is_borrowed = 0;
    memmove(book_node->properties, &is_borrowed, sizeof(int8_t));

    int new_rating = *(int *)(book_node->properties + sizeof(int8_t) +
                        sizeof(int)) + rating;
    memmove(book_node->properties + sizeof(int8_t) + sizeof(int),
            &new_rating, sizeof(int));

    int purchases = *(int *)(book_node->properties + sizeof(int8_t)) + 1;
    memmove(book_node->properties + sizeof(int8_t), &purchases, sizeof(int));

    // Marks to user->properties that he has borrowed a book and actualize his
    // points.
    memmove(user_node->properties + sizeof(int8_t), &is_borrowed,
            sizeof(int8_t));
    memmove(user_node->properties + 2 * sizeof(int8_t),
            &actual_points, sizeof(int));

    if (actual_points < 0) {
        int8_t permission = 0;
        memmove(user_node->properties, &permission, sizeof(int8_t));
        printf("The user %s has been banned from this library.\n", user_name);
        return;
    }
}

/*
* A function to punish the ones that does not take care of books.
* The book is foreverly lost and this can not be undone.
* It is deleted from the system and its datas as well.
* The user will be punished with -50 points added to his score.
*/
void lost_book(hashtable_t *users, hashtable_t *library) {
    char user_name[MAX_KEY_VAL_USER_SIZE];
    char book_name[MAX_BOOK_NAME];

    scanf("%s", user_name);
    read_dict_name(book_name);

    if (!ht_has_key(users, user_name)) {
        printf("You are not registered yet.\n");
        return;
    }

    unsigned int user_id = users->hash_function(user_name) % users->hmax;
    ll_node_t *user_node = return_key_node(users, user_name, user_id);

    // Verify permission.
    if (!(*(int8_t *)(user_node->properties))) {
        printf("You are banned from this library.\n");
        return;
    }

    unsigned int book_id = library->hash_function(book_name) % library->hmax;
    delete_book_node(library, book_name, book_id);

    int new_points = (*(int *)(user_node->properties +
                    2 * sizeof(int8_t))) - 50;
    memmove(user_node->properties + 2 * sizeof(int8_t), &new_points,
            sizeof(int));

    int8_t borrowed = 0;
    memmove(user_node->properties + sizeof(int8_t), &borrowed, sizeof(int8_t));

    if (new_points < 0) {
        int8_t permission = 0;
        memmove(user_node->properties, &permission, sizeof(int8_t));
        printf("The user %s has been banned from this library.\n", user_name);
        return;
    }
}
