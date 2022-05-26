// Copyright 2022 Avram Cristian-Stefan

#ifndef _HOME_STUDENT_BOOK_H_
#define _HOME_STUDENT_BOOK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/home/student/struct.h"

#define MAX_COMMAND_SIZE 30
#define MAX_SYMBOL_SIZE 30

void change_def(hashtable_t *definitions, char *key, char *val);
void initialize_book_data(hashtable_t **library, char *name);
void initialize_book_data(hashtable_t **library, char *name);
void add_book(hashtable_t **library, int *library_created);
void get_or_remove_def(hashtable_t *library, int option);
void add_def(hashtable_t *library);
void remove_book(hashtable_t **library);
void get_book(hashtable_t *library);

#endif  //  _HOME_STUDENT_BOOK_H_
