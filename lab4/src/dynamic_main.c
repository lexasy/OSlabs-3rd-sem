#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *handler = dlopen("./realization1.so", RTLD_LAZY);
    if (!handler) {
        fputs(dlerror(), stderr);
        exit(1);
    }
    int realization = 0;
    int cmd;
    char *error;
    while (printf("Enter the command (0 - changing realization (library), 1 - calculating integral of sin, 2 - calulating Pi number): ") > 0 && scanf("%d", &cmd) != EOF) {
        if (cmd == 0) {
            if (dlclose(handler) != 0) {
                perror("dlclose");
                exit(1);
            }
            realization = (realization == 1 ? 2 : 1);
            handler = (realization == 1 ? dlopen("./realization1.so", RTLD_LAZY) : dlopen("./realization2.so", RTLD_LAZY));
            if (!handler) {
                fputs(dlerror(), stderr);
                exit(1);
            }
            printf("Realization was changed from realization%d to realization%d.\n", (realization == 1 ? 2 : 1), realization);
        } else if (cmd == 1) {
            float (*SinIntegral)(float, float, float) = dlsym(handler, "SinIntegral");
            if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "%s\n", error);
                exit(1);
            }
            float A, B, e;
            printf("Enter the arguments (begin of the segment, end of the segment, step): ");
            scanf("%f %f %f", &A, &B, &e);
            printf("Result: %f\n", SinIntegral(A, B, e));
        } else if (cmd == 2) {
            float (*Pi)(int) = dlsym(handler, "Pi");
            if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "%s\n", error);
                exit(1);
            }
            int K;
            printf("Enter the quantity of elements in the row: ");
            scanf("%d", &K);
            printf("Result: %f\n", Pi(K));
        } else {
            printf("Wrong command! Try again!\n");
        }
    }
    if (dlclose(handler) != 0) {
        perror("dlclose");
        exit(1);
    }
    exit(0);
}