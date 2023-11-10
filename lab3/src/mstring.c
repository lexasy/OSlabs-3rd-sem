#include "string.h"

my_string *create_string() {
    my_string *tmp = (my_string *) malloc(sizeof(my_string));
    tmp->str = (char *) malloc(sizeof(char) * DEFAULT);
    tmp->length = 0; return tmp;
}

void read_string(my_string *mstr) {
    char c;
    while ((c = getchar()) != 10) {
        mstr->str[mstr->length] = c; mstr->length++;
    }
}

void print_string(my_string *mstr) {
    for (int j = 0; j < mstr->length; j++) {
        printf("%c", mstr->str[j]);
    }
    printf("\n");
}