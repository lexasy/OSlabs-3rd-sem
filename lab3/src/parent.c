#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define MEM_SIZE 4096

char tolowerc(char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
}

void delete_double_spaces(char *mstr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (mstr[i] == 32 && mstr[i + 1] == 32) {
            mstr[i] = 0;
        }
    }
}

pid_t create_process() {
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        exit(-1);
    }
    return pid;
}

int main() {
    int fd = open("file", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    ftruncate(fd, MEM_SIZE);
    char *file_in_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    my_string *p_mstr = create_string();
    printf("Enter your string: "); read_string(p_mstr);
    for (int i = 0; i < p_mstr->length; i++) {
        file_in_memory[i] = p_mstr->str[i];
    }
    close(fd);
    pid_t cp1, cp2;
    if ((cp1 = create_process()) == 0) { //child1
        // file_in_memory[0] = tolowerc(file_in_memory[0]);
        // print_string(&file_in_memory[0]);
        for (int i = 0; i < p_mstr->length; i++) {
            file_in_memory[i] = tolowerc(file_in_memory[i]);
        }
        munmap(file_in_memory, MEM_SIZE);
        exit(0);
        // execl("../build/child1", "../build/child1", NULL);
    } else if (cp1 > 0 && (cp2 = create_process()) == 0) { //child2
        delete_double_spaces(file_in_memory, p_mstr->length);
        munmap(file_in_memory, MEM_SIZE);
        exit(0);
        // execl("../build/child2", "../build/child2", NULL);
    } else { //parent
        wait(NULL);
        printf("Result: "); 
        for (int i = 0; i < p_mstr->length; i++) {
            printf("%c", file_in_memory[i]);
        }
        printf("\n");
        munmap(file_in_memory, MEM_SIZE);
    }
    return 0;
}
