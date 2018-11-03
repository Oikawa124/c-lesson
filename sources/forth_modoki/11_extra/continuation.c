#include "clesson.h"

static struct Continuation co_stack[STACK_SIZE];
static int co_pos = 0;

void co_push(struct Continuation *cont) {
    co_stack[co_pos] = *cont;
    co_pos++;
}

void co_push_elem_arr(struct Element *elem) {
    struct Continuation cont = {elem->u.byte_codes, .pc=0};
    co_push(&cont);
}

void co_pop() {
    co_pos--;
    if (co_pos < 0) { fprintf(stderr, "co_pos is minus"); }
}

struct Continuation *co_peek() {
    return &co_stack[co_pos - 1];
}

int co_stack_is_empty() {
    if (co_pos == 0) {
        return 1;
    } else {
        return 0;
    }
}

void set_current_op_pos(int out_op_pos) {
    co_stack[co_pos - 1].pc = out_op_pos;
}