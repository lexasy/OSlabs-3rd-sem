#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define MEM_SIZE 4096

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
    int fd_count = open("count", O_RDWR | O_CREAT, 0666);
    if (fd_count == -1) {
        perror("open");
        return -1;
    }
    ftruncate(fd, MEM_SIZE);
    ftruncate(fd_count, MEM_SIZE);
    char *file_in_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    int *count_in_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_count, 0);
    if (count_in_memory == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    char *argv[] = {"file", "count", (char *) NULL};
    my_string *p_mstr = create_string();
    printf("Enter your string: "); read_string(p_mstr);
    for (int i = 0; i < p_mstr->length; i++) {
        file_in_memory[i] = p_mstr->str[i];
    }
    count_in_memory[0] = p_mstr->length;
    close(fd); close(fd_count);
    pid_t cp1, cp2;
    if ((cp1 = create_process()) == 0) { //child1
        if (execv("../build/child1", argv) == -1) {
            perror("execv");
            return -1;
        }
    } else if (cp1 > 0 && (cp2 = create_process()) == 0) { //child2
        if (execv("../build/child2", argv) == -1) {
            perror("execv");
            return -1;
        }
    } else { //parent
        wait(NULL);
        printf("Result: "); 
        for (int i = 0; i < count_in_memory[0]; i++) {
            printf("%c", file_in_memory[i]);
        }
        printf("\n");
        munmap(count_in_memory, MEM_SIZE);
        munmap(file_in_memory, MEM_SIZE);
    }
    return 0;
}
