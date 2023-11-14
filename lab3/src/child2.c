#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MEM_SIZE 4096

void delete_double_spaces(char *mstr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (mstr[i] == 32 && mstr[i + 1] == 32) {
            mstr[i] = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    int fd = open(argv[0], O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    int fd_count = open(argv[1], O_RDWR, 0666);
    if (fd_count == -1) {
        perror("open");
        return -1;
    }
    ftruncate(fd, MEM_SIZE); ftruncate(fd_count, MEM_SIZE);
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
    close(fd); close(fd_count);
    delete_double_spaces(file_in_memory, count_in_memory[0]);
    munmap(count_in_memory, MEM_SIZE);
    munmap(file_in_memory, MEM_SIZE);
    return 0;
}