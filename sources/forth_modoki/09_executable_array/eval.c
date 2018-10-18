#include "clesson.h"
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#define MAX_NAME_OP_NUMBERS 256

void eval_exec_array(struct ElementArray *elmarr);

static void preprocessing_four_arithmetic_operations(int *out_num1, int *out_num2){

    struct Element num1 = {NO_ELEMENT, {0}};
    struct Element num2 = {NO_ELEMENT, {0}};

    stack_pop(&num1);
    stack_pop(&num2);

    *out_num1 = num1.u.number;
    *out_num2 = num2.u.number;
}

static void def_op(){
    struct Element val = {NO_ELEMENT, {0}};
    stack_pop(&val);

    struct Element literal_name = {NO_ELEMENT, {0}};
    stack_pop(&literal_name);

    dict_put(literal_name.u.name, &val);
}

static void add_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1+ num2;

    stack_push(&answer);
}

static void sub_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 - num1;

    stack_push(&answer);
}

static void mul_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num1 * num2;

    stack_push(&answer);
}

static void div_op(){
    int num1, num2;
    preprocessing_four_arithmetic_operations(&num1, &num2);

    struct Element answer = {ELEMENT_NUMBER, {0}};
    answer.u.number = num2 / num1;

    stack_push(&answer);
}

static int compile_exec_array(int ch, struct Element *out_elem){
    struct Element arr[MAX_NAME_OP_NUMBERS];
    struct Token token = {UNKNOWN, {0}};

    int i=0;

    do{
        ch = get_next_token(ch, &token);
        switch (token.ltype) {
            case NUMBER:
                arr[i].etype = ELEMENT_NUMBER;
                arr[i].u.number = token.u.number;
                i++;
                break;
            case LITERAL_NAME:
                arr[i].etype = ELEMENT_LITERAL_NAME;
                arr[i].u.name = token.u.name;
                i++;
                break;
            case EXECUTABLE_NAME:
                arr[i].etype = ELEMENT_EXECUTABLE_NAME;
                arr[i].u.name = token.u.name;
                i++;
                break;
            case OPEN_CURLY: {
                struct Element nest_elem = {NO_ELEMENT, {0}};
                compile_exec_array(ch, &nest_elem);
                arr[i].etype = nest_elem.etype;
                arr[i].u.byte_codes = nest_elem.u.byte_codes;
                i++;
                ch = parse_one(ch, &token);
                break;
            }
        }
    }while (ch != '}');

    struct ElementArray *elem_arr = (struct EelementArray*)malloc(sizeof(struct ElementArray)+sizeof(struct Element)*i);

    elem_arr->len = i;

    memcpy(elem_arr->elements, arr, sizeof(struct Element)*i);

    out_elem->etype = ELEMENT_EXECUTABLE_ARRAY;
    out_elem->u.byte_codes = elem_arr;

    return ch;
}


void eval(){
    static int ch = EOF;

    do{
        struct Token token = {UNKNOWN, {0}};
        struct Element elem = {NO_ELEMENT, {0}};

        ch = get_next_token(ch, &token);

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
                    if (elem.etype == ELEMENT_C_FUNC) {
                        elem.u.cfunc();
                        break;
                    } else if (elem.etype == ELEMENT_EXECUTABLE_ARRAY) {
                        eval_exec_array(elem.u.byte_codes);
                        break;
                    } else {
                        stack_push(&elem);
                        break;
                    }
                }
            case OPEN_CURLY:
                ch = compile_exec_array(ch, &elem);
                stack_push(&elem);
                break;
        }
    }while (ch != EOF);
}

void eval_exec_array(struct ElementArray *elmarr) {
    set_exec_array_to_parser(elmarr);
    eval();
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

static void test_eval_executable_array_one_number(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value = {ELEMENT_NUMBER, {1}};

    char *input = "{1}";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect_array_value.etype == actual.u.byte_codes->elements[0].etype);
    assert(expect_array_value.u.number == actual.u.byte_codes->elements[0].u.number);

    stack_clear();
}

static void test_eval_executable_array_literal_name(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value = {ELEMENT_LITERAL_NAME, {.name="abc"}};

    char *input = "{/abc}";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect_array_value.etype == actual.u.byte_codes->elements[0].etype);
    assert(streq(expect_array_value.u.name, actual.u.byte_codes->elements[0].u.name));

    stack_clear();
}

static void test_eval_executable_array_executable_name(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value = {ELEMENT_EXECUTABLE_NAME, {.name="abc"}};

    char *input = "{abc}";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect_array_value.etype == actual.u.byte_codes->elements[0].etype);
    assert(streq(expect_array_value.u.name, actual.u.byte_codes->elements[0].u.name));

    stack_clear();
}

static void test_eval_executable_array_two_numbers(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value1 = {ELEMENT_NUMBER, {1}};
    struct Element expect_array_value2 = {ELEMENT_NUMBER, {2}};

    char *input = "{1 2}";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect_array_value1.etype == actual.u.byte_codes->elements[0].etype);
    assert(expect_array_value1.u.number == actual.u.byte_codes->elements[0].u.number);

    assert(expect_array_value2.etype == actual.u.byte_codes->elements[1].etype);
    assert(expect_array_value2.u.number == actual.u.byte_codes->elements[1].u.number);

    stack_clear();
}

static void test_eval_two_executable_arrays(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value1 = {ELEMENT_NUMBER, {1}};
    struct Element expect_array_value2 = {ELEMENT_NUMBER, {2}};

    char *input = "{1} {2}";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    stack_pop(&actual2);
    stack_pop(&actual1);

    assert(expect.etype == actual1.etype);
    assert(expect_array_value1.etype == actual1.u.byte_codes->elements[0].etype);
    assert(expect_array_value1.u.number == actual1.u.byte_codes->elements[0].u.number);

    assert(expect.etype == actual2.etype);
    assert(expect_array_value2.etype == actual2.u.byte_codes->elements[0].etype);
    assert(expect_array_value2.u.number == actual2.u.byte_codes->elements[0].u.number);

    stack_clear();
}

static void test_eval_nest_executable_arrays(){

    struct Element expect = {ELEMENT_EXECUTABLE_ARRAY, {0}};
    struct Element expect_array_value1 = {ELEMENT_NUMBER, {1}};
    struct Element expect_array_value2 = {ELEMENT_NUMBER, {2}};
    struct Element expect_array_value3 = {ELEMENT_NUMBER, {3}};

    char *input = "{1 {2} 3}";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect_array_value1.etype == actual.u.byte_codes->elements[0].etype);
    assert(expect_array_value1.u.number == actual.u.byte_codes->elements[0].u.number);

    assert(expect_array_value2.etype == actual.u.byte_codes->elements[1].u.byte_codes->elements[0].etype);
    assert(expect_array_value2.u.number == actual.u.byte_codes->elements[1].u.byte_codes->elements[0].u.number);

    assert(expect_array_value3.etype == actual.u.byte_codes->elements[2].etype);
    assert(expect_array_value3.u.number == actual.u.byte_codes->elements[2].u.number);
    stack_clear();
}

static void test_eval_executable_array_action(){

    struct Element expect = {ELEMENT_NUMBER, {2}};

    char *input = "/a {1 1 add} def a";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}

static void test_eval_nested_executable_array_action(){

    struct Element expect = {ELEMENT_NUMBER, {3}};

    char *input = "/ZZ {6} def /YY {4 ZZ 5} def /XX {1 2 YY 3} def XX";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}


static void unit_test(){
//    test_eval_push_number_to_stack();
//    test_eval_add();
//    test_eval_add_with_many_values();
//    test_eval_dict();

//    test_eval_def_and_stack_pop();
//    test_eval_sub();
//    test_eval_mul();
//    test_eval_div();

//    test_eval_executable_array_one_number();
//    test_eval_executable_array_literal_name();
//    test_eval_executable_array_executable_name();
//    test_eval_executable_array_two_numbers();
//    test_eval_two_executable_arrays();
//    test_eval_nest_executable_arrays();
    test_eval_executable_array_action();
    test_eval_nested_executable_array_action();
}


int main() {
    stack_init();
    register_primitives();

    unit_test();

//    dict_print_all();
//    stack_print_all();
    return 1;
}

// TODO 実行可能配列を実装していく。