#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "string.h"


pid_t create_process() {
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        exit(-1);
    }
    return pid;
}

void my_dup(int old_fd, int new_fd) {
    if (dup2(old_fd, new_fd) == -1) {
        perror("dup2");
        exit(-1);
    }
}

void my_pipe(int pipe_fd[]) {
    int err = pipe(pipe_fd);
    if (-1 == err) {
        perror("pipe");
        exit(-1);
    }
}

int main() {
    int pipe_fd1[2]; int pipe_fd2[2]; int pipe_fd3[2];
    my_pipe(pipe_fd1); my_pipe(pipe_fd2); my_pipe(pipe_fd3);
    pid_t cp1, cp2;
    if ((cp1 = create_process()) == 0) { //child1
        if (close(pipe_fd1[1]) == -1 || close(pipe_fd2[0]) == -1) {return -1;}
        my_dup(pipe_fd1[0], STDIN_FILENO); my_dup(pipe_fd2[1], STDOUT_FILENO);
        execl("../build/child1", "../build/child1", NULL);
        if (close(pipe_fd1[0]) == -1 || close(pipe_fd2[1]) == -1) {return -1;}
    } else if (cp1 > 0 && (cp2 = create_process()) == 0) { //child2
        if (close(pipe_fd2[1]) == -1|| close(pipe_fd3[0]) == -1) {return -1;} 
        my_dup(pipe_fd2[0], STDIN_FILENO); my_dup(pipe_fd3[1], STDOUT_FILENO);
        execl("../build/child2", "../build/child2", NULL);
        if (close(pipe_fd2[0]) == -1 || close(pipe_fd3[1]) == -1) {return -1;}
    } else { //parent
        my_string *p_mstr = create_string();
        printf("Enter your string: "); read_string(p_mstr);
        if (close(pipe_fd1[0]) == -1 || close(pipe_fd3[1]) == -1) {return -1;}
        write(pipe_fd1[1], &(p_mstr->length), sizeof(int));
        write(pipe_fd1[1], p_mstr->str, sizeof(char) * p_mstr->length);
        read(pipe_fd3[0], &(p_mstr->length), sizeof(int));
        read(pipe_fd3[0], p_mstr->str, sizeof(char) * p_mstr->length);
        if (close(pipe_fd1[1]) == -1 || close(pipe_fd3[0]) == -1) {return -1;}
        printf("Result: "); print_string(p_mstr);
    }
    return 0;
}
