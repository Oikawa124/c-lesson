#include "clesson.h"
#include <stdio.h>
#include <assert.h>

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


void eval(){
    static int ch = EOF;

    do{
        struct Token token = {UNKNOWN, {0}};

        ch = parse_one(ch, &token);

        switch (token.ltype) {
            case NUMBER:
                stack_push(&token);
                break;
            case EXECUTABLE_NAME:
                if (streq(token.u.name, "add")){
                    add_op();
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

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert_elem_number(expect1, &actual1);

    stack_clear();
}


static void unit_test(){
    test_eval_push_number_to_stack();
    test_eval_add();
    test_eval_add_with_many_values();
}

int main() {
    stack_init();
    unit_test();

    return 1;
}
