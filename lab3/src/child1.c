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

void tolowerc(my_string *mstr) {
    for (int i = 0; i < mstr->length; i++) {
        if (mstr->str[i] >= 'A' && mstr->str[i] <= 'Z') {
            mstr->str[i] += 32;
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
    tolowerc(&file_in_memory[0]);
    munmap(file_in_memory, sizeof(my_string));
    return 0;
    // my_string *c1_mstr = create_string();
    // read(STDIN_FILENO, &(c1_mstr->length), sizeof(int));
    // read(STDIN_FILENO, c1_mstr->str, sizeof(char) * c1_mstr->length);
    // tolowerc(c1_mstr);
    // write(STDOUT_FILENO, &(c1_mstr->length), sizeof(int));
    // write(STDOUT_FILENO, c1_mstr->str, sizeof(char) * c1_mstr->length);
    // close(STDIN_FILENO); close(STDOUT_FILENO);
    // return 0;
}