#include <stdio.h>
#include <stdlib.h>
#include "clesson.h"

char words[100];
int word_pos = 0;

void add_char(int ch){
    words[word_pos] = ch;
    word_pos++;
    if (ch == '\0'){
        word_pos =0;
    }
}

char *get_readline(FILE *f_p){
    int ch;
    char *ret;

    while ((ch = getc(f_p)) != EOF){
        if (ch == '\n'){
            add_char('\0');
            break;
        }
        add_char(ch);
    }
    if (ch == EOF){
        add_char(EOF);
    }

    ret = malloc(sizeof(char) * 100);
    strcpy(ret, words);

    return ret;
}

int main(int argc, char *argv[]){

    char *buf;

    if (argc >= 2) {
        fp = fopen("text", "r");
        if (fp == NULL) {
            fprintf(stderr, "NO EXIST FILE");
        }
        init();

        eval();

        stack_print_all();

        fclose(fp);
    } else {
        init();

        printf("input words here\n<");
        while ((buf = get_readline(stdin)) != NULL) {
            cl_getc_set_src(buf);
            eval();
            stack_print_all();
            printf("<");
        }
    }

    return 0;
}

