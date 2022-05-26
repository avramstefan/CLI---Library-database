// Copyright 2022 Avram Cristian-Stefan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "/home/student/exit.h"
#include "/home/student/struct.h"
#include "/home/student/utils.h"

// Function used to swap two names, ratings and purchases with the given idx
void swap_for_ratings(char **names, float *float_ratings, int *purchases,
                        int i, int j) {
    char *tmp = names[i];
    names[i] = names[j];
    names[j] = tmp;

    float temp = float_ratings[i];
    float_ratings[i] = float_ratings[j];
    float_ratings[j] = temp;

    int temp2 = purchases[i];
    purchases[i] = purchases[j];
    purchases[j] = temp2;
}

/*
* This function takes the names, purchases and ratings of every book.
* The purchases and ratings are converted to float_ratings as:
*           float_rating = rating / purchases;
* After comparing by float_rating and names, using the swap function from above, 
* the datas will be printed at STDOUT.
*/
void show_lib_ranking(hashtable_t *library, int library_created) {
    printf("Books ranking:\n");

    if (!library_created)
        return;

    char **names = (char **)malloc(library->size * sizeof(char *));
    int *ratings = (int *)malloc(library->size * sizeof(int));
    int *purchases = (int *)malloc(library->size * sizeof(int));
    int len = 0;
    int size = library->size;

    for (unsigned int i = 0; i < library->hmax; i++) {
        if (library->buckets[i]->head != NULL) {
            ll_node_t *node = library->buckets[i]->head;

            if (library->buckets[i]->size > 1) {
                size += (library->buckets[i]->size - 1);
                names = realloc(names, size * sizeof(char *));
                ratings = realloc(ratings, size * sizeof(int));
                purchases = realloc(purchases, size * sizeof(int));
            }

            while (node != NULL) {
                info *datas = (info *)node->data;
                char *key = (char *)datas->key;

                if (!node->properties) {
                    node = node->next;
                    continue;
                }

                names[len] = malloc(strlen(key) + 1);
                memmove(names[len], key, strlen(key) + 1);
                memmove(&ratings[len], &(*(int *)(node->properties +
                        sizeof(int8_t) + sizeof(int))), sizeof(int));
                memmove(&purchases[len], &(*(int *)(node->properties +
                        sizeof(int8_t))), sizeof(int));
                node = node->next;
                len++;
            }
        }
    }
    float *float_ratings = calloc(len, sizeof(float));

    for (int i = 0; i < len; i++) {
        if (purchases[i])
            float_ratings[i] = (float)ratings[i] / (float)purchases[i];
    }

    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (float_ratings[i] < float_ratings[j]) {
                swap_for_ratings(names, float_ratings, purchases, i, j);
            } else if (float_ratings[i] == float_ratings[j]) {
                if (purchases[i] < purchases[j]) {
                    swap_for_ratings(names, float_ratings, purchases, i, j);
                } else if (purchases[i] == purchases[j]) {
                    if (strcmp(names[j], names[i]) < 0)
                        swap_for_ratings(names, float_ratings,
                                        purchases, i, j);
                }
            }
        }
    }

    for (int i = 0; i < len; i++) {
        char formatted_name[MAX_BOOK_NAME];
        snprintf(formatted_name, MAX_BOOK_NAME, "%s", names[i]);
        formatted_name[strlen(formatted_name) - 1] = '\0';
        printf("%d. Name:%s Rating:%.3f Purchases:%d\n", i + 1,
                formatted_name + 1, float_ratings[i], purchases[i]);
    }

    free(float_ratings);
    for (int i = 0; i < len; i++)
        free(names[i]);
    free(names);
    free(ratings);
    free(purchases);
}

// Function to swap two names and points by the given indexes
void swap_ranking_user(char **names, int *points, int i, int j) {
    char *tmp = names[i];
    names[i] = names[j];
    names[j] = tmp;

    int temp = points[i];
    points[i] = points[j];
    points[j] = temp;
}

/*
* This function takes the names and points of every user.
* After comparing by points and names, using the swap function from above, 
* the datas will be printed at STDOUT.
*/
void show_user_ranking(hashtable_t *users, int user_created) {
    printf("Users ranking:\n");

    if (!user_created)
        return;

    char **names = malloc(users->size * sizeof(char *));
    int *points = malloc(users->size * sizeof(int));
    int len = 0;
    int size = users->size;

    for (unsigned int i = 0; i < users->hmax; i++) {
        if (users->buckets[i]->head != NULL) {
            ll_node_t *node = users->buckets[i]->head;

            if (users->buckets[i]->size > 1) {
                size += (users->buckets[i]->size - 1);
                names = realloc(names, size * sizeof(char *));
                points = realloc(points, size * sizeof(int));
            }

            while (node != NULL) {
                if (!(*(int8_t *)(node->properties))) {
                    node = node->next;
                    continue;
                }

                info *datas = (info *)node->data;
                char *key = (char *)datas->key;

                names[len] = malloc(strlen(key) + 1);
                memmove(names[len], key, strlen(key) + 1);
                memmove(&points[len], &(*(int *)(node->properties +
                        2 * sizeof(int8_t))), sizeof(int));
                node = node->next;
                len++;
            }
        }
    }

    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (points[i] < points[j]) {
                swap_ranking_user(names, points, i, j);
            } else if (points[i] == points[j]) {
                if (strcmp(names[j], names[i]) < 0) {
                    swap_ranking_user(names, points, i, j);
                }
            }
        }
    }

    for (int i = 0; i < len; i++) {
        printf("%d. Name:%s Points:%d\n", i + 1, names[i], points[i]);
    }

    for (int i = 0; i < len; i++)
        free(names[i]);
    free(names);
    free(points);
}
