#include <unistd.h>
#include "string.h"

void tolowerc(my_string *mstr) {
    for (int i = 0; i < mstr->length; i++) {
        if (mstr->str[i] >= 'A' && mstr->str[i] <= 'Z') {
            mstr->str[i] += 32;
        }
    }
}

int main() {
    my_string *c1_mstr = create_string();
    read(STDIN_FILENO, &(c1_mstr->length), sizeof(int));
    read(STDIN_FILENO, c1_mstr->str, sizeof(char) * c1_mstr->length);
    tolowerc(c1_mstr);
    write(STDOUT_FILENO, &(c1_mstr->length), sizeof(int));
    write(STDOUT_FILENO, c1_mstr->str, sizeof(char) * c1_mstr->length);
    close(STDIN_FILENO); close(STDOUT_FILENO);
    return 0;
}