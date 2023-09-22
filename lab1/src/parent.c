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

int main() {
    int pipe_fd1[2]; int pipe_fd2[2]; int pipe_fd3[2];
    int err1 = pipe(pipe_fd1);
    int err2 = pipe(pipe_fd2);
    int err3 = pipe(pipe_fd3);
    if (-1 == err1 || -1 == err2 || -1 == err3) {
        perror("pipe");
        return -1;
    }

    pid_t cp1, cp2;
    if ((cp1 = create_process()) == 0) { //child1
        close(pipe_fd1[1]); close(pipe_fd2[0]);
        my_dup(pipe_fd1[0], STDIN_FILENO);
        my_dup(pipe_fd2[1], STDOUT_FILENO);
        execl("../build/child1", "../build/child1", NULL);
        close(pipe_fd1[0]); close(pipe_fd2[1]);
    } else if (cp1 > 0 && (cp2 = create_process()) == 0) { //child2
        close(pipe_fd2[1]); close(pipe_fd3[0]);
        my_dup(pipe_fd2[0], STDIN_FILENO);
        my_dup(pipe_fd3[1], STDOUT_FILENO);
        execl("../build/child2", "../build/child2", NULL);
        close(pipe_fd2[0]); close(pipe_fd3[1]);
    } else { //parent
        my_string *p_mstr = create_string(); //int a;
        printf("Enter your string: ");
        read_string(p_mstr);
        close(pipe_fd1[0]); close(pipe_fd3[1]);
        write(pipe_fd1[1], &(p_mstr->length), sizeof(int));
        write(pipe_fd1[1], p_mstr->str, sizeof(char) * p_mstr->length);
        read(pipe_fd3[0], &(p_mstr->length), sizeof(int));
        read(pipe_fd3[0], p_mstr->str, sizeof(char) * p_mstr->length);
        close(pipe_fd1[1]); close(pipe_fd3[0]);
        printf("Result: "); print_string(p_mstr);
    }
    return 0;
}
