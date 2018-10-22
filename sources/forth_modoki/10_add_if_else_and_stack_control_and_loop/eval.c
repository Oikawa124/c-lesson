#include "clesson.h"
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#define MAX_NAME_OP_NUMBERS 256

void eval_exec_array();

static int compile_exec_array(int ch, struct Element *out_elem){
    struct Element arr[MAX_NAME_OP_NUMBERS];
    struct Token token = {UNKNOWN, {0}};
    int cur_op_pos = 0;

    int i=0;

    do{
        ch = get_next_token(ch, &token, &cur_op_pos);
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
                ch = get_next_token(ch, &token, &cur_op_pos);
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
    int cur_op_pos=0;

    do{
        struct Token token = {UNKNOWN, {0}};
        struct Element elem = {NO_ELEMENT, {0}};

        ch = get_next_token(ch, &token, &cur_op_pos);

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
                        co_push_elem_arr(&elem);
                        eval_exec_array();
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

void eval_exec_array() {
    int ch = EOF;
    struct Token token = {UNKNOWN, 0};
    struct Element elem = {NO_ELEMENT, {0}};
    int cur_op_pos=0;

    while (get_stack_pos() >= 1){
        struct Continuation *cont = co_peek();
        set_cont(cont);

        do{
            ch = get_next_token(ch, &token, &cur_op_pos);

            if (token.ltype == NUMBER) {
                elem.etype = ELEMENT_NUMBER;
                elem.u.number = token.u.number;
                stack_push(&elem);

            }else if (token.ltype == LITERAL_NAME){
                elem.etype = ELEMENT_LITERAL_NAME;
                elem.u.name = token.u.name;
                stack_push(&elem);

            }else if (token.ltype == EXECUTABLE_NAME) {
                if (dict_get(token.u.name, &elem) != -1){
                    if (elem.etype == ELEMENT_C_FUNC) {
                        elem.u.cfunc();
                    } else if (elem.etype == ELEMENT_EXECUTABLE_ARRAY) {
                        set_current_op_pos(cur_op_pos);
                        cur_op_pos = 0;
                        co_push_elem_arr(&elem);
                        break;
                    } else {
                        stack_push(&elem);
                    }
                }
            }
        }while (ch != EOF);
    }
}




void assert_number_eq(int expect, struct Element *actual){
    assert(expect == actual->u.number);
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

    assert_number_eq(expect1, &actual1);
    assert_number_eq(expect2, &actual2);

    stack_clear();
}

static void test_eval_add(){
    int expect1 = 3;

    char *input = "1 2 add";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_number_eq(expect1, &actual1);

    stack_clear();
}

static void test_eval_add_with_many_values(){
    int expect1 = 45;

    char *input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect1, &actual);

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

    assert_number_eq(expect_dict_value, &actual_dict);

    stack_clear();

}

static void test_eval_def_and_stack_pop(){
    int expect_value = 12;

    char *input = "/abc 12 def abc";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};
    stack_pop(&actual);

    assert_number_eq(expect_value, &actual);

    stack_clear();

}

static void test_eval_sub(){
    int expect1 = 2;

    char *input = "3 1 sub";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_number_eq(expect1, &actual1);

    stack_clear();
}

static void test_eval_mul(){
    int expect1 = 10;

    char *input = "5 2 mul";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_number_eq(expect1, &actual1);

    stack_clear();
}

static void test_eval_div(){
    int expect1 = 3;

    char *input = "9 3 div";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_number_eq(expect1, &actual1);

    stack_clear();
}

static void test_eval_eq(){
    int expect = 1;

    char *input = "1 1 eq";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}


static void test_eval_neq(){
    int expect = 1;

    char *input = "1 2 neq";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_gt(){
    int expect = 1;

    char *input = "10 2 gt";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_ge(){
    int expect = 1;

    char *input = "2 2 ge";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_lt(){
    int expect = 1;

    char *input = "1 2 lt";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_le(){
    int expect = 1;

    char *input = "1 1 le";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_pop(){
    int expect = 1;

    char *input = "1 1 pop";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_exch(){
    int expect1 = 2;
    int expect2 = 1;

    char *input = "1 2 exch";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    stack_pop(&actual2);
    stack_pop(&actual1);

    assert_number_eq(expect1, &actual1);
    assert_number_eq(expect2, &actual2);

    stack_clear();
}

static void test_eval_dup(){
    int expect1 = 1;
    int expect2 = 1;

    char *input = "1 dup";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);
    stack_pop(&actual2);

    assert_number_eq(expect1, &actual1);
    assert_number_eq(expect2, &actual2);

    stack_clear();
}

static void test_eval_index(){
    int expect = 3;

    char *input = "1 2 3 4 5 2 index";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);


    stack_clear();
}

static void test_eval_exec(){
    int expect = 10;

    char *input = "{5 5 add} exec";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_if(){
    int expect = 3;

    char *input = "1 {1 2 add} if";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_ifelse(){
    int expect = 10;

    char *input = "0 {5 5 add} {3 2 add} ifelse";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_repeat(){
    int expect = 11;

    char *input = "1 2 {5 add} repeat";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

    stack_clear();
}

static void test_eval_while(){
    int expect = 0;

    char *input = "1 1 1 {pop} {1} while";
    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert_number_eq(expect, &actual);

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

static void test_eval_nested_executable_array_action1(){

    struct Element expect = {ELEMENT_NUMBER, {2}};

    char *input = "/a {1 add} def /b {1 a} def b";

    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}

static void test_eval_nested_executable_array_action2(){

    int expect_array_no0 = 1;
    int expect_array_no1 = 2;
    int expect_array_no2 = 4;
    int expect_array_no3 = 6;
    int expect_array_no4 = 5;
    int expect_array_no5 = 3;

    // 出力 : 1, 2, 4, 6, 5, 3
    char *input = "/ZZ {6} def /YY {4 ZZ 5} def /XX {1 2 YY 3} def XX";


    cl_getc_set_src(input);

    eval();

    struct Element actual0= {NO_ELEMENT, {0}};
    struct Element actual1= {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};
    struct Element actual3 = {NO_ELEMENT, {0}};
    struct Element actual4 = {NO_ELEMENT, {0}};
    struct Element actual5 = {NO_ELEMENT, {0}};

    stack_pop(&actual5);
    stack_pop(&actual4);
    stack_pop(&actual3);
    stack_pop(&actual2);
    stack_pop(&actual1);
    stack_pop(&actual0);

    assert_number_eq(expect_array_no0, &actual0);
    assert_number_eq(expect_array_no1, &actual1);
    assert_number_eq(expect_array_no2, &actual2);
    assert_number_eq(expect_array_no3, &actual3);
    assert_number_eq(expect_array_no4, &actual4);
    assert_number_eq(expect_array_no5, &actual5);

    stack_clear();
}

static void test_eval_nested_executable_array_action3(){

    struct Element expect = {ELEMENT_NUMBER, {6}};

    char *input = "/a {1 add} def /b {1 a} def /c {1 b add 2 mul} def c ";

    cl_getc_set_src(input);

    eval();

    struct Element actual = {NO_ELEMENT, {0}};

    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

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
    test_eval_eq();
    test_eval_neq();
    test_eval_gt();
    test_eval_ge();
    test_eval_lt();
    test_eval_le();
    test_eval_pop();
    test_eval_exch();
    test_eval_dup();
    test_eval_index();
    test_eval_exec();
    test_eval_if();
    test_eval_ifelse();
    test_eval_repeat();
    test_eval_while();


    test_eval_executable_array_one_number();
    test_eval_executable_array_literal_name();
    test_eval_executable_array_executable_name();
    test_eval_executable_array_two_numbers();
    test_eval_two_executable_arrays();
    test_eval_nest_executable_arrays();
    test_eval_executable_array_action();
    test_eval_nested_executable_array_action1();
    test_eval_nested_executable_array_action2();
    test_eval_nested_executable_array_action3();
}


int main() {
    stack_init();
    register_primitives();

    unit_test();

//    dict_print_all();
//    stack_print_all();
    return 1;
}
