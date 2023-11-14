#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MEM_SIZE 4096

char tolowerc(char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
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
    for (int i = 0; i < count_in_memory[0]; i++) {
        file_in_memory[i] = tolowerc(file_in_memory[i]);
    }
    munmap(count_in_memory, MEM_SIZE);
    munmap(file_in_memory, MEM_SIZE);
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