#include "clesson.h"
#include <stdio.h>
#include <assert.h>

// opは、operatorのこと
void add_op(){
    struct Element answer = {NO_ELEMENT, {0}};
    struct Element num1 = {NO_ELEMENT, {0}};
    struct Element num2 = {NO_ELEMENT, {0}};

    stack_pop(&num1);
    stack_pop(&num2);

    answer.u.number = num1.u.number + num2.u.number;
    answer.etype = NUMBER;

    stack_push(&answer);
}


void def_op(){
    struct Element val = {UNKNOWN, {0}};
    stack_pop(&val);

    struct Element literal_name = {UNKNOWN, {0}};
    stack_pop(&literal_name);

    dict_put(literal_name.u.name, &val);
}

void eval(){
    static int ch = EOF;

    do{
        struct Token token = {UNKNOWN, {0}};
        struct Element val = {UNKNOWN, {0}};

        ch = parse_one(ch, &token);

        switch (token.ltype) {
            case NUMBER:
                stack_push(&token);
                break;
            case LITERAL_NAME:
                stack_push(&token);
                break;
            case EXECUTABLE_NAME:
                if (streq("add", token.u.name)){
                    add_op();
                    break;
                } else if (streq("def", token.u.name)){
                    def_op();
                    break;
                } else if (dict_get(token.u.name, &val) != -1){
                    stack_push(&val);
                    break;
                }
        }
    }while (ch != EOF);
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
    dict_clean();
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
    dict_clean();
}

static void unit_test(){
    test_eval_push_number_to_stack();
    test_eval_add();
    test_eval_add_with_many_values();
    test_eval_dict();
    test_eval_def_and_stack_pop();
}


int main() {
    stack_init();
    dict_init();
    unit_test();

//    dict_print_all();
//    stack_print_all();

    return 1;
}