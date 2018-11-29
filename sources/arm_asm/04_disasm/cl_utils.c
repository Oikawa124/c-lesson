#include <stdarg.h>
#include <stdio.h>
#include "disasm.h"


static char buf[100*1024];

static int buf_pos[1000];

static int to_buffer = 0;
static int pos = 0;
static int pre_pos = 0;

static int call_print_time = 0;

void cl_enable_buffer_mode() {
    to_buffer = 1;
}

char *cl_get_printed_buffer() {
    return &buf[pre_pos];
}

char *cl_get_result(int num) {
    return &buf[buf_pos[num-1]];
}

void cl_printf(char *fmt, ...) {
    pre_pos = pos;


    va_list arg_ptr;
    va_start(arg_ptr, fmt);

    if(to_buffer) {
        pos += vsprintf(&buf[pos], fmt, arg_ptr);
        pos++;
        buf_pos[call_print_time] = pos;
        call_print_time++;

    } else {
        vprintf(fmt, arg_ptr);
    }
    va_end(arg_ptr);
}
