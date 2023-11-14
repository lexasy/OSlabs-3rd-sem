#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void delete_double_spaces(my_string *mstr) {
    int len = mstr->length;
    for (int i = 0; i < len; i++) {
        if (mstr->str[i] == 32 && mstr->str[i + 1] == 32) {
            mstr->str[i] = 0;
        }
    }
}

int main() {
    int fd = open("file.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    ftruncate(fd, sizeof(my_string));
    my_string *file_in_memory = (my_string *)mmap(NULL, sizeof(my_string), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    close(fd);
    delete_double_spaces(&file_in_memory[0]);
    munmap(file_in_memory, sizeof(my_string));
    return 0;
    // my_string *c2_mstr = create_string();
    // read(STDIN_FILENO, &(c2_mstr->length), sizeof(int));
    // read(STDIN_FILENO, c2_mstr->str, sizeof(char) * c2_mstr->length);
    // delete_double_spaces(c2_mstr);
    // write(STDOUT_FILENO, &(c2_mstr->length), sizeof(int));
    // write(STDOUT_FILENO, c2_mstr->str, sizeof(char) * c2_mstr->length);
    // close(STDIN_FILENO); close(STDOUT_FILENO);
    // return 0;
}