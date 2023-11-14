#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>

#define MEM_SIZE 4096

char tolowerc(char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
}

int main(int argc, char *argv[]) {
    int fd, fd_count, fd_sem;
    char *file_in_memory; int *count_in_memory; sem_t *semaphore_in_memory;
    if (((fd = open(argv[0], O_RDWR, 0666)) == -1) || ((fd_count = open(argv[1], O_RDWR, 0666)) == -1) || ((fd_sem = open(argv[2], O_RDWR, 0666)) == -1)) {
        perror("open");
        exit(-1);
    }
    if ((ftruncate(fd, MEM_SIZE) == -1) || (ftruncate(fd_count, MEM_SIZE) == -1) || (ftruncate(fd_sem, MEM_SIZE) == -1)) {
        perror("ftruncate");
        exit(-1);
    }
    if (((file_in_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) || ((count_in_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_count, 0)) == MAP_FAILED) || ((semaphore_in_memory = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_sem, 0)) == MAP_FAILED)) {
        perror("mmap");
        exit(-1);
    }
    if ((close(fd) == -1) || (close(fd_count) == -1) || (close(fd_sem) == -1)) {
        perror("close");
        exit(-1);
    }
    sem_wait(semaphore_in_memory);
    for (int i = 0; i < count_in_memory[0]; i++) {
        file_in_memory[i] = tolowerc(file_in_memory[i]);
    }
    sem_post(semaphore_in_memory);
    if ((munmap(count_in_memory, MEM_SIZE) == -1) || (munmap(file_in_memory, MEM_SIZE) == -1) || (munmap(semaphore_in_memory, sizeof(sem_t)) == -1)) {
        perror("munmap");
        exit(-1);
    }
    return 0;
}