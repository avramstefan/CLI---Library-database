// Copyright 2022 Avram Cristian-Stefan

#ifndef _HOME_STUDENT_USER_H_
#define _HOME_STUDENT_USER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/home/student/struct.h"

#define MAX_COMMAND_SIZE 30
#define MAX_SYMBOL_SIZE 30

void initialize_user_data(hashtable_t **users, char *name);
void add_user(hashtable_t **users, int *user_created);
void borrow(hashtable_t *users, hashtable_t *library,
            int user_created, int library_created);
void return_book(hashtable_t *users, hashtable_t *library);
void lost_book(hashtable_t *users, hashtable_t *library);

#endif  //  _HOME_STUDENT_USER_H_
