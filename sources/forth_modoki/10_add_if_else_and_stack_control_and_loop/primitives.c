#include "clesson.h"

static void preprocessing_four_arithmetic_operations(int *out_num1, int *out_num2){

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
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1+ num2;

    stack_push(&answer);
}

void sub_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 - num1;

    stack_push(&answer);
}

void mul_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1 * num2;

    stack_push(&answer);
}

void div_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 / num1;

    stack_push(&answer);
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
}