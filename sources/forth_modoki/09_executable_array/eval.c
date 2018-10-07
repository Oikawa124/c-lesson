#include "clesson.h"
#include <stdio.h>
#include <assert.h>


void preprocessing_four_arithmetic_operations(int *out_num1, int *out_num2){

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


void eval(){
    static int ch = EOF;

    do{
        struct Token token = {UNKNOWN, {0}};
        struct Element elem = {NO_ELEMENT, {0}};

        ch = parse_one(ch, &token);

        switch (token.ltype) {
            case NUMBER:
                elem.etype = ELEMENT_NUMBER;
                elem.u.number = token.u.number;
                stack_push(&elem);
                break;
            case LITERAL_NAME:
                elem.etype = ELEMENT_LITERAL_NAME;
                elem.u.name = token.u.name;
                stack_push(&elem);
                break;
            case EXECUTABLE_NAME:
                if (dict_get(token.u.name, &elem) != -1){
                    if (elem.etype == ELEMENT_C_FUNC){
                        elem.u.cfunc();
                    } else {
                        stack_push(&elem);
                        break;
                    }
                }
        }
    }while (ch != EOF);
}

void register_one_primitive(char *name, void (*cfunc)(void)){
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

void assert_elem_number(int number, struct Element *elm){
    assert(number == elm->u.number);
}

static void test_eval_push_number_to_stack(){
    int expect1 = 1;
    int expect2 = 2;

    char *input = "1 2";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    stack_pop(&actual2);
    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);
    assert_elem_number(expect2, &actual2);

    stack_clear();
}

static void test_eval_add(){
    int expect1 = 3;

    char *input = "1 2 add";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);

    stack_clear();
}

static void test_eval_add_with_many_values(){
    int expect1 = 45;

    char *input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_elem_number(expect1, &actual);

    stack_clear();
}

static void test_eval_dict(){
    char *input_key = "abc";
    int expect_dict_value = 12;

    char *input = "/abc 12 def";
    cl_getc_set_src(input);

    eval();

    struct Element actual_dict = {NO_ELEMENT, {0}};
    dict_get(input_key, &actual_dict);

    assert_elem_number(expect_dict_value, &actual_dict);

    stack_clear();

}

static void test_eval_def_and_stack_pop(){
    int expect_value = 12;

    char *input = "/abc 12 def abc";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};
    stack_pop(&actual);

    assert_elem_number(expect_value, &actual);

    stack_clear();

}

static void test_eval_sub(){
    int expect1 = 2;

    char *input = "3 1 sub";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);

    stack_clear();
}

static void test_eval_mul(){
    int expect1 = 10;

    char *input = "5 2 mul";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);

    stack_clear();
}

static void test_eval_div(){
    int expect1 = 3;

    char *input = "9 3 div";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);

    stack_clear();
}


static void unit_test(){
    test_eval_push_number_to_stack();
    test_eval_add();
    test_eval_add_with_many_values();
    test_eval_dict();
    test_eval_def_and_stack_pop();
    test_eval_sub();
    test_eval_mul();
    test_eval_div();
}


int main() {
    stack_init();
    register_primitives();

    unit_test();

//    dict_print_all();
//    stack_print_all();

    return 1;
}