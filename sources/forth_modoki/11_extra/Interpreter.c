#include "clesson.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    FILE *fp;

    init();

    if (argc >= 2) {
        fp = fopen("fibonacci_2.ps", "r"); //argv[1]
        cl_getc_set_fp(fp);
        if (fp == NULL) {
            fprintf(stderr, "NO EXIST FILE");
            exit(1);
        }
        eval();

        stack_print_all();

        fclose(fp);
    } else {
        repl();
    }
    return 0;
}

