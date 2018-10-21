#include "clesson.h"

static struct Continuation co_stack[STACK_SIZE];
static int co_pos=0;

void co_push(struct Continuation *cont){
    co_stack[co_pos] = *cont;
    co_pos++;
}

void co_pop(){
    co_pos--;
}

struct Continuation *co_peek(){
    return &co_stack[co_pos - 1];
}

int get_stack_pos(){
    return co_pos;
}

void set_current_op_pos(int out_op_pos){
    co_stack[co_pos-1].pc = out_op_pos;
}
