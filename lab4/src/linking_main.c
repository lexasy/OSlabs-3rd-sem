#include "foos.h"
#include <stdio.h>

int main() {
    int cmd;
    while (printf("Enter the command (1 - calculating integral of sin, 2 - calulating Pi number): ") > 0 && scanf("%d", &cmd) != EOF) {
        if (cmd == 1) {
            float A, B, e;
            printf("Enter the arguments (begin of the segment, end of the segment, step): ");
            scanf("%f %f %f", &A, &B, &e);
            printf("Result: %f\n", SinIntegral(A, B, e));
        } else if (cmd == 2) {
            int K;
            printf("Enter the quantity of elements in the row: ");
            scanf("%d", &K);
            printf("Result: %f\n", Pi(K));
        } else {
            printf("Wrong command! Try again!\n");
        }
    }
}