#include <stdio.h>
#include <assert.h>

#define STACK_SIZE 1024


enum ElementType {
    NUMBER,
    LITERAL_NAME,
    NO_ELEMENT
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char *name;
    } u;
};


static struct Element stack[STACK_SIZE];
static int stack_pos;

void stack_push(struct Element *token){
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

void stack_print_all(){
    int _stack_pos = stack_pos - 1;

    if (stack_pos == 1){
        printf("NO ELEMENT");
        return;
    }

    do{
        switch(stack[_stack_pos].etype){
            case NUMBER:
                printf("%d:: num: %d\n",_stack_pos, stack[_stack_pos].u.number);
                break;
            case LITERAL_NAME:
                printf("%d:: LITERAL_NAME: %s\n",_stack_pos, stack[_stack_pos].u.name);
                break;
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
    struct Element expect2 = {NUMBER, {3}};

    struct Element actual1 = {NUMBER, {3}};
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
    struct Element actual = {NUMBER, {3}};

    stack_push(&actual);

    assert(stack[stack_pos - 1].etype == actual.etype);
    assert(stack[stack_pos - 1].u.number == actual.u.number);

    stack_clear();
}

static void test_stack_pop_one_times(){
    struct Element expect = {NUMBER, {3}};

    stack_push(&expect);

    struct Element actual = {NO_ELEMENT, {0}};
    stack_pop(&actual);

    assert(expect.etype == actual.etype);
    assert(expect.u.number == actual.u.number);

    stack_clear();
}

static void test_stack_pop_two_times(){
    struct Element expect1 = {NUMBER, {3}};
    struct Element expect2 = {LITERAL_NAME, {"add"}};

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


int main() {
    stack_init();
    unit_tests();


    struct Element element1 = {NUMBER, {3}};
    struct Element element2 = {LITERAL_NAME, {"add"}};
    struct Element element3 = {LITERAL_NAME, {"plus"}};
    struct Element element4 = {NUMBER, {10}};
    stack_push(&element1);
    stack_push(&element2);
    stack_push(&element3);
    stack_push(&element4);

    stack_print_all();

    return 1;
}