#include "clesson.h"

static FILE *fp;

static char *input = NULL;
static int pos = 0;

void cl_getc_set_fp(FILE *_fp) {
    fp = _fp;
}

void cl_getc_set_src(char *str) {
    input = str;
    pos = 0;
}

int cl_getc() {
    if (fp) {
        return fgetc(fp);
    } else {
        if (strlen(input) == pos)
            return EOF;
        return input[pos++];
    }
}
