#include "clesson.h"
#include <stdio.h>
#include <assert.h>



void eval(){
    static int ch = EOF;
    stack_init();

    do{
        struct Token token = {UNKNOWN, {0}};

        ch = parse_one(ch, &token);


        switch (token.ltype) {
            case NUMBER:
                stack_push(&token);
                break;
            case EXECUTABLE_NAME:
                if (streq(token.u.name, "add")){
                    stack_add();
                    break;
                }
        }


    }while (ch != EOF);
}

static void test_eval_push_number_to_stack(){
    struct Element expect1 = {NUMBER, {1}};
    struct Element expect2 = {NUMBER, {2}};

    char *input = "1 2";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    stack_pop(&actual2);
    stack_pop(&actual1);

    assert(expect1.etype == actual1.etype);
    assert(expect1.u.number == actual1.u.number);
    assert(expect2.etype == actual2.etype);
    assert(expect2.u.number == actual2.u.number);

    stack_clear();
}

static void test_eval_add(){
    struct Element expect1 = {NUMBER, {3}};

    char *input = "1 2 add";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert(expect1.etype == actual1.etype);
    assert(expect1.u.number == actual1.u.number);

    stack_clear();
}

static void test_eval_add_with_many_values(){
    struct Element expect1 = {NUMBER, {45}};

    char *input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    cl_getc_set_src(input);

    eval();

    struct Element actual1 = {NO_ELEMENT, {0}};

    stack_pop(&actual1);

    assert(expect1.etype == actual1.etype);
    assert(expect1.u.number == actual1.u.number);

    stack_clear();
}


static void unit_test(){
    test_eval_push_number_to_stack();
    test_eval_add();
    test_eval_add_with_many_values();
}

int main() {

    unit_test();

    return 1;
}
