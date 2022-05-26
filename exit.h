// Copyright 2022 Avram Cristian-Stefan

#ifndef _HOME_STUDENT_EXIT_H_
#define _HOME_STUDENT_EXIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "/home/student/struct.h"

#define MAX_SYMBOL_SIZE 30

void swap_for_ratings(char **names, float *float_ratings,
                    int *purchases, int i, int j);
void show_lib_ranking(hashtable_t *library, int library_created);
void swap_ranking_user(char **names, int *points, int i, int j);
void show_user_ranking(hashtable_t *users, int user_created);

#endif  //  _HOME_STUDENT_EXIT_H_
