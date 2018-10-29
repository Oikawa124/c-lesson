#include "clesson.h"

static void preprocessing_operations(int *out_num1, int *out_num2){

    struct Element num1 = {NO_ELEMENT, {0}};
    struct Element num2 = {NO_ELEMENT, {0}};

    stack_pop(&num1);
    stack_pop(&num2);

    *out_num1 = num1.u.number;
    *out_num2 = num2.u.number;
}

void def_op(){
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

    struct Element literal_name = {NO_ELEMENT, {0}};
    stack_pop(&literal_name);

    dict_put(literal_name.u.name, &val);
}

void add_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1+ num2;

    stack_push(&answer);
}

void sub_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 - num1;

    stack_push(&answer);
}

void mul_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1 * num2;

    stack_push(&answer);
}

void div_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 / num1;

    stack_push(&answer);
}

void eq_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 == num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void neq_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 != num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void gt_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 < num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void ge_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 <= num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void lt_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 > num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void le_op(){
    int num1, num2;
    preprocessing_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    if (num1 >= num2) {
        answer.u.number = 1;
    } else {
        answer.u.number = 0;
    }

    stack_push(&answer);
}

void pop_op(){
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

}

void exch_op(){
    struct Element val1 = {NO_ELEMENT, {0}};
    struct Element val2= {NO_ELEMENT, {0}};
    stack_pop(&val1);
    stack_pop(&val2);

    stack_push(&val1);
    stack_push(&val2);
}

void dup_op(){
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

    stack_push(&val);
    stack_push(&val);

}

void index_op(){
    struct Element index = {NO_ELEMENT, {0}};
    struct Element val = {NO_ELEMENT, {0}};

    stack_pop(&index);

    stack_value_copy(&val, index.u.number);

    stack_push(&val);
}

void request_execute(struct ElementArray *execarr){

}

void exec_op() {
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

    co_push_elem_arr(&val);
    eval_exec_array();
}


void if_op(){
    struct Element bool = {NO_ELEMENT, {0}};
    struct Element proc = {NO_ELEMENT, {0}};
    stack_pop(&proc);
    stack_pop(&bool);

    if (bool.u.number == 1){
        co_push_elem_arr(&proc);
        eval_exec_array();
    }

}

void ifelse_op(){
    struct Element bool = {NO_ELEMENT, {0}};
    struct Element proc1 = {NO_ELEMENT, {0}};
    struct Element proc2 = {NO_ELEMENT, {0}};

    stack_pop(&proc2);
    stack_pop(&proc1);
    stack_pop(&bool);

    if (bool.u.number == 1){
        co_push_elem_arr(&proc2);
        eval_exec_array();
    } else {
        co_push_elem_arr(&proc1);
        eval_exec_array();
    }

}

void repeat_op(){
    struct Element n = {NO_ELEMENT, {0}};
    struct Element proc = {NO_ELEMENT, {0}};

    stack_pop(&proc);
    stack_pop(&n);

    for (int i=0; i < n.u.number; i++){
        co_push_elem_arr(&proc);
        eval_exec_array();
    }
}

static void while_op(){
    struct Element cond = {NO_ELEMENT, {0}};
    struct Element body = {NO_ELEMENT, {0}};
    stack_pop(&body);
    stack_pop(&cond);

    co_push_elem_arr(&cond);
    eval_exec_array();

    struct Element val = {NO_ELEMENT, {0}};
//    printf("$$$$$$$$$$$$$$$ while_op\n");
//    stack_print_all();
//    printf("$$$$$$$$$$$$$$$\n");

    stack_pop(&val);

    while (val.etype == NUMBER && val.u.number == 1) {
//        printf("################\n");
//        stack_print_all();

        co_push_elem_arr(&body);
        eval_exec_array();

        co_push_elem_arr(&cond);
        eval_exec_array();

        stack_pop(&val);
    }
}



static void register_one_primitive(char *name, void (*cfunc)(void)){
    struct Element elem = {ELEMENT_C_FUNC, {.cfunc = cfunc}};
    dict_put(name, &elem);
}

void register_primitives(){
    register_one_primitive("add", add_op);
    register_one_primitive("sub", sub_op);
    register_one_primitive("mul", mul_op);
    register_one_primitive("div", div_op);

    register_one_primitive("def", def_op);

    register_one_primitive("eq", eq_op);
    register_one_primitive("neq", neq_op);
    register_one_primitive("gt", gt_op);
    register_one_primitive("ge", ge_op);
    register_one_primitive("lt", lt_op);
    register_one_primitive("le", le_op);

    register_one_primitive("pop", pop_op);
    register_one_primitive("exch", exch_op);
    register_one_primitive("dup", dup_op);
    register_one_primitive("index", index_op);

    register_one_primitive("exec", exec_op);
    register_one_primitive("if", if_op);
    register_one_primitive("ifelse", ifelse_op);
    register_one_primitive("repeat", repeat_op);
    register_one_primitive("while", while_op);
}
