#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"

#define _R_            "\x1b[31m" // red colour for the first child process output colour
#define _G_            "\x1b[32m" // green colour for parent process output colour
#define _Y_            "\x1b[33m" // yellow colour for the second child process output colour
#define _B_            "\x1b[34m" //blue colour for input colour
#define RESET_COLOR    "\x1b[0m"  // reset colour

#define DEFAULT 80

typedef struct {
    char *str;
    int length;
} my_string;

my_string *create_string() {
    my_string *tmp = (my_string *) malloc(sizeof(my_string));
    tmp->str = (char *) malloc(sizeof(char) * DEFAULT);
    tmp->length = 0; return tmp;
}

void tolowerc(my_string *mstr) {
    for (int i = 0; i < mstr->length; i++) {
        if (mstr->str[i] >= 'A' && mstr->str[i] <= 'Z') {
            mstr->str[i] += 32;
        }
    }
}

void delete_double_spaces(my_string *mstr) {
    for (int i = 0; i < mstr->length; i++) {
        if (mstr->str[i] == 32 && mstr->str[i + 1] == 32) {
            mstr->str[i] = 0;
        }
    }
}

void read_string(my_string *mstr) {
    char c;
    while ((c = getchar()) != 10) {
        mstr->str[mstr->length] = c; mstr->length++;
    }
}

void print_string(my_string *mstr) {
    for (int j = 0; j < mstr->length; j++) {
        printf("%c", mstr->str[j]);
    }
    printf("\n");
}

pid_t create_process() {
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        exit(-1);
    }
    return pid;
}

int main() {
    my_string *p_mstr = create_string();
    printf(_B_"Enter your string: ");
    read_string(p_mstr);
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
        printf(_R_"Hello! I'm the first child process. I need to receive your string from the parent process and take it to lower register.\n");
        my_string *c1_mstr = create_string();
        close(pipe_fd1[1]);
        close(pipe_fd2[0]);
        read(pipe_fd1[0], c1_mstr, sizeof(my_string));
        tolowerc(c1_mstr);
        printf(_R_"Your string is: "); print_string(c1_mstr);
        write(pipe_fd2[1], c1_mstr, sizeof(my_string));
        printf(_R_"I've sent your string to the second child process, please wait it...\n");
        close(pipe_fd1[0]); close(pipe_fd2[1]);
    } else if (cp1 > 0 && (cp2 = create_process()) == 0) { //child2
        printf(_Y_"Hello! I'm the second child process. I need to receive your string from the first child process and delete double spaces.\n");
        my_string *c2_mstr = create_string();
        close(pipe_fd2[1]); close(pipe_fd3[0]);
        read(pipe_fd2[0], c2_mstr, sizeof(my_string));
        delete_double_spaces(c2_mstr);
        printf(_Y_"Your string is: "); print_string(c2_mstr);
        write(pipe_fd3[1], c2_mstr, sizeof(my_string));
        printf(_Y_"I've sent your string to the parent process, please wait it...\n");
        close(pipe_fd2[0]); close(pipe_fd3[1]);
    } else { //parent
        printf(_G_"Hello! I'm a parent process. I need to send your string to the first child process.\n");
        close(pipe_fd1[0]); close(pipe_fd3[1]);
        write(pipe_fd1[1], p_mstr, sizeof(my_string));
        close(pipe_fd1[1]);
        printf(_G_"I've sent your string to the first child. Result waiting...\n");
        wait(NULL);
        read(pipe_fd3[0], p_mstr, sizeof(my_string));
        close(pipe_fd3[0]);
        printf(_G_"The children processess finished.\n");
        printf(_G_"Your result is: "); print_string(p_mstr);
        printf(_G_"Goodbye!\n");
    }
}
