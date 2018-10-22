#include "clesson.h"
#include <string.h>

static char *input = NULL;
size_t len =0;

static int pos = 0;

void cl_getc_set_src(char* str){
    input = str;
    pos = 0;
}

void textfile_to_input(FILE *fp){
    getdelim(&input, &len, '\0', fp);
    pos = 0;
}

int cl_getc() {
    if(strlen(input) == pos)
        return EOF;
    int num = input[pos++];

    if (num == '\n') {
        num = ' ';
    } else if (num == '%') {
        while (input[pos] != '\n'){
            pos++;
        }
        num = ' ';
    }

    return num;
}
