// Copyright 2022 Avram Cristian-Stefan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "/home/student/struct.h"
#include "/home/student/book.h"
#include "/home/student/user.h"
#include "/home/student/exit.h"
#include "/home/student/utils.h"

int main(void) {
    hashtable_t *users, *library;
    int user_created = 0, library_created = 0;

    char command[MAX_COMMAND_SIZE];

    while (1) {
        scanf("%s", command);

        if (strncmp(command, "ADD_USER", 8) == 0) {
            add_user(&users, &user_created);
        } else if (strncmp(command, "EXIT", 4) == 0) {
            show_lib_ranking(library, library_created);
            show_user_ranking(users, user_created);
            ht_free(library, library_created, 1);
            ht_free(users, user_created, 2);
            break;
        } else if (strncmp(command, "ADD_BOOK", 8) == 0) {
            add_book(&library, &library_created);
        } else if (strncmp(command, "GET_DEF", 7) == 0) {
            get_or_remove_def(library, 1);
        } else if (strncmp(command, "RMV_DEF", 7) == 0) {
            get_or_remove_def(library, 2);
        } else if (strncmp(command, "ADD_DEF", 7) == 0) {
            add_def(library);
        } else if (strncmp(command, "RMV_BOOK", 8) == 0) {
            remove_book(&library);
        } else if (strncmp(command, "BORROW", 6) == 0) {
            borrow(users, library, user_created, library_created);
        } else if (strncmp(command, "RETURN", 6) == 0) {
            return_book(users, library);
        } else if (strncmp(command, "GET_BOOK", 8) == 0) {
            get_book(library);
        } else if (strncmp(command, "LOST", 4) == 0) {
            lost_book(users, library);
        }
    }

    return 0;
}
