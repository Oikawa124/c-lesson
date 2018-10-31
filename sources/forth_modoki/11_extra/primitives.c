#include <malloc.h>
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




void exec_op() {
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

    request_execute(&val);
}


void if_op(){
    struct Element bool = {NO_ELEMENT, {0}};
    struct Element proc = {NO_ELEMENT, {0}};
    stack_pop(&proc);
    stack_pop(&bool);

    if (bool.u.number == 1){
        request_execute(&proc);
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
        request_execute(&proc2);
    } else {
        request_execute(&proc1);
    }

}

void repeat_op(){
    struct Element n = {NO_ELEMENT, {0}};
    struct Element proc = {NO_ELEMENT, {0}};

    stack_pop(&proc);
    stack_pop(&n);

    struct Element exec = {ELEMENT_EXECUTABLE_NAME, {.name="exec"}};


    struct ElementArray *elem_arr = (struct EelementArray*)malloc(sizeof(struct ElementArray)+sizeof(struct Element)*(n.u.number*2));
    elem_arr->len = n.u.number*2;

    for (int i = 0; i < elem_arr->len; i +=2){
        elem_arr->elements[i] = proc;
        elem_arr->elements[i+1] = exec;
    }

    struct Element val = {ELEMENT_EXECUTABLE_ARRAY, {.byte_codes = elem_arr}};

    request_execute(&val);
}


static void while_op(){
    struct Element cond = {NO_ELEMENT, {0}};
    struct Element body = {NO_ELEMENT, {0}};
    stack_pop(&body);
    stack_pop(&cond);

    struct Element exec = {ELEMENT_EXECUTABLE_NAME, {.name="exec"}};
    struct Element jmp = {ELEMENT_JMP, {0}};
    struct Element jmp_not_if = {ELEMENT_JMP_NOT_IF, {0}};
    struct Element number_5 = {ELEMENT_NUMBER, {.number=5}};
    struct Element number_minus_7 = {ELEMENT_NUMBER, {.number=-7}};


    struct ElementArray *elem_arr = (struct EelementArray*)malloc(sizeof(struct ElementArray)+sizeof(struct Element)*8);
    elem_arr->len = 8;

    elem_arr->elements[0] = cond;
    elem_arr->elements[1] = exec;
    elem_arr->elements[2] = number_5;
    elem_arr->elements[3] = jmp_not_if;
    elem_arr->elements[4] = body;
    elem_arr->elements[5] = exec;
    elem_arr->elements[6] = number_minus_7;
    elem_arr->elements[7] = jmp;

    struct Element val = {ELEMENT_EXECUTABLE_ARRAY, {.byte_codes = elem_arr}};

    request_execute(&val);
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
