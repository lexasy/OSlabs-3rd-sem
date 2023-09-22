#include <stdio.h>
#include <stdlib.h>

#define DEFAULT 80

typedef struct {
    char *str;
    int length;
} my_string;

my_string *create_string();
void read_string(my_string *mstr);
void print_string(my_string *mstr);

