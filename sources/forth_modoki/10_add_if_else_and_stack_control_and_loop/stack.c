#include "clesson.h"
#include <stdio.h>
#include <assert.h>

static struct Element stack[STACK_SIZE];
static int stack_pos;

void stack_push(struct Element *token){
    if (stack_pos >STACK_SIZE ){
        printf("stack_pos is larger than STACK_SIZE");
    }

    stack[stack_pos] = *token;
    stack_pos++;
}

void stack_pop(struct Element *out_element){
    if (stack_pos == 1){
        *out_element = stack[stack_pos - 1];
    } else {
        stack_pos--;
        *out_element = stack[stack_pos];
    }
}

void stack_value_copy(struct Element *out_element, int index){
        *out_element = stack[stack_pos - index - 1];
}


void stack_print_all(){
    int _stack_pos = stack_pos - 1;

    if (stack_pos == 1){
        printf("NO ELEMENT\n");
        return;
    }

    do{
        switch(stack[_stack_pos].etype){
            case ELEMENT_NUMBER:
                printf("%d:: num: %d\n",_stack_pos, stack[_stack_pos].u.number);
                break;
            case ELEMENT_LITERAL_NAME:
                printf("%d:: ELEMENT_LITERAL_NAME: %s\n",_stack_pos, stack[_stack_pos].u.name);
                break;
            case ELEMENT_EXECUTABLE_ARRAY:
                printf("%d:: ELEMENT_EXECUTABLE_ARRAY\n", _stack_pos);
                int i=0;
                printf("##\n");
                do {
                    switch(stack[_stack_pos].u.byte_codes->elements[i].etype)
                    {
                        case ELEMENT_NUMBER:
                            printf("%d:::: num: %d\n",i, stack[_stack_pos].u.byte_codes->elements[i].u.number);
                            break;
                        case ELEMENT_LITERAL_NAME:
                            printf("%d:::: ELEMENT_LITERAL_NAME: %s\n",i, stack[_stack_pos].u.byte_codes->elements[i].u.name);
                            break;
                        case ELEMENT_EXECUTABLE_NAME:
                            printf("%d:::: ELEMENT_EXECUTABLE_NAME: %s\n",i, stack[_stack_pos].u.byte_codes->elements[i].u.name);
                            break;
                        case ELEMENT_EXECUTABLE_ARRAY:
                            printf("%d:::: ELEMENT_EXECUTABLE_ARRAY\n", i);
                            break;
                        default:
                            printf("default");
                            break;
                    }
                    i++;
                } while(i < stack[_stack_pos].u.byte_codes->len);
                printf("##\n");
                break;
            default:
                printf("UNKNOWN_ELEMENT_TYPE\n");
        }
        _stack_pos--;
    }while (_stack_pos >= 1);
}

void stack_clear() {
    stack_pos = 1;
}

void stack_init(){
    stack_pos = 1;
    stack[stack_pos - 1].etype = NO_ELEMENT;
    stack[stack_pos - 1].u.number = 0;
}


static void test_stack_pop_when_stack_has_no_elements(){

    struct Element expect = {NO_ELEMENT, {0}};

    struct Element actual = {NO_ELEMENT, {0}};
    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}

static void test_stack_pop_when_stack_has_no_elements_two_times(){

    struct Element expect1 = {NO_ELEMENT, {0}};
    struct Element expect2 = {NO_ELEMENT, {0}};

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

static void test_stack_pop_when_stack_has_no_elements_and_push_one_times(){

    struct Element expect1 = {NO_ELEMENT, {0}};
    struct Element expect2 = {ELEMENT_NUMBER, {3}};

    struct Element actual1 = {ELEMENT_NUMBER, {3}};
    struct Element actual2 = {NO_ELEMENT, {0}};
    stack_pop(&actual2);
    stack_push(&actual1);

    assert(expect1.etype == actual2.etype);
    assert(expect1.u.number == actual2.u.number);

    assert(expect2.etype == stack[stack_pos - 1].etype);
    assert(expect2.u.number == stack[stack_pos - 1].u.number);

    stack_clear();
}

static void test_stack_push(){
    struct Element actual = {ELEMENT_NUMBER, {3}};

    stack_push(&actual);

    assert(stack[stack_pos - 1].etype == actual.etype);
    assert(stack[stack_pos - 1].u.number == actual.u.number);

    stack_clear();
}

static void test_stack_pop_one_times(){
    struct Element expect = {ELEMENT_NUMBER, {3}};

    stack_push(&expect);

    struct Element actual = {NO_ELEMENT, {0}};
    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}

static void test_stack_pop_two_times(){
    struct Element expect1 = {ELEMENT_NUMBER, {3}};
    struct Element expect2 = {ELEMENT_LITERAL_NAME, {"add"}};

    stack_push(&expect1);
    stack_push(&expect2);

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


static void unit_tests(){
    test_stack_pop_when_stack_has_no_elements();
    test_stack_pop_when_stack_has_no_elements_two_times();
    test_stack_pop_when_stack_has_no_elements_and_push_one_times();
    test_stack_push();
    test_stack_pop_one_times();
    test_stack_pop_two_times();
}

#if 0
int main() {
    stack_init();
    unit_tests();


    struct Element element1 = {ELEMENT_NUMBER, {3}};
    struct Element element2 = {ELEMENT_LITERAL_NAME, {"add"}};
    struct Element element3 = {ELEMENT_LITERAL_NAME, {"plus"}};
    struct Element element4 = {ELEMENT_NUMBER, {10}};
    stack_push(&element1);
    stack_push(&element2);
    stack_push(&element3);
    stack_push(&element4);

    stack_print_all();

    return 1;
}
#endif