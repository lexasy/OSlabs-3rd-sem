#include <unistd.h>
#include "string.h"

void delete_double_spaces(my_string *mstr) {
    int len = mstr->length;
    for (int i = 0; i < len; i++) {
        if (mstr->str[i] == 32 && mstr->str[i + 1] == 32) {
            mstr->str[i] = 0;
        }
    }
}

int main() {
    my_string *c2_mstr = create_string();
    read(STDIN_FILENO, &(c2_mstr->length), sizeof(int));
    read(STDIN_FILENO, c2_mstr->str, sizeof(char) * c2_mstr->length);
    delete_double_spaces(c2_mstr);
    write(STDOUT_FILENO, &(c2_mstr->length), sizeof(int));
    write(STDOUT_FILENO, c2_mstr->str, sizeof(char) * c2_mstr->length);
    close(STDIN_FILENO); close(STDOUT_FILENO);
    return 0;
}