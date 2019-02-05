#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "asm.h"

#define BUF_SIZE 100
FILE *fp;


static char *input = NULL;
static int pos = 0;

void cl_getc_set_src(char *str) {
    input = str;
    pos = 0;
}

static int cl_getc() {
    if (fp) {
        return fgetc(fp);
    } else {
        if (strlen(input) == pos)
            return EOF;
        return input[pos++];
    }
}


// g_は"global"
static char g_buf[BUF_SIZE];


// 一行読み込み
int cl_getline(char **out_buf) {
    int len = 0;
    int ch;

    if ((ch = cl_getc()) == EOF) {return EOF;}

    do {
        g_buf[len] = (char)ch;
        len++;

    } while ((ch = cl_getc()) != '\n' && ch != EOF);

    g_buf[len] = '\0';

    *out_buf = g_buf;

    return len;
}