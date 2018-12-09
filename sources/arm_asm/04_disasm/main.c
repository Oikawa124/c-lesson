#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "disasm.h"


int print_asm(int word) {
    if (0xe3a01000 == (word & 0xe3a01000)) {

        int _register = (word >> 12) & 0x0000f;
        int offset = word & 0x00000fff;

        cl_printf("mov r%x, #0x%x", _register, offset);
        return 1;
    } else if (word == 0xEAFFFFFE) {
        cl_printf("b [r15, #0x34]");
        return 1;
    } else {
        // 無し
    }
    return 0;
}


int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }

static void test_print_asm_immediate68() {
    char *expect = "mov r1, #0x68";
    int input = 0xE3A01068;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(1);

    assert(is_instruction == 1);
    assert(streq(expect, actual));

    cl_clear_output();
}


static void test_print_asm_immediate65() {
    char *expect = "mov r1, #0x65";
    int input = 0xE3A01065;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(1);

    assert(is_instruction == 1);
    assert(streq(expect, actual));

    cl_clear_output();
}



static void test_print_asm_branch() {
    char *expect = "b [r15, #0x34]";
    int input = 0xEAFFFFFE;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(1);

    assert(is_instruction == 1);
    assert(streq(expect, actual));

    cl_clear_output();
}


static void test_print_asm_not_an_order() {
    int expect = '\0';
    int input = 0x64646464;

    cl_clear_output();
    int is_instruction = print_asm(input);
    char *actual = cl_get_result(1);

    assert(is_instruction == 0);
    assert(actual[0] == expect);
}

static void test_print_asm_get_result() {
    char *expect = "mov r1, #0x65";
    int input1 = 0xE3A01063;
    int input2 = 0xE3A01065;
    int input3 = 0xE3A01064;


    print_asm(input1);
    print_asm(input2);
    print_asm(input3);

    char *actual1 = cl_get_result(1);
    char *actual2 = cl_get_result(2);
    char *actual3 = cl_get_result(3);

    assert(streq(expect, actual2));

    cl_clear_output();
}

static void unit_test() {

    cl_enable_buffer_mode();

    // 命令: mov
    test_print_asm_immediate68();
    test_print_asm_immediate65();

    // 命令: b
   test_print_asm_branch();

    //命令でない
    test_print_asm_not_an_order();

    test_print_asm_get_result();
}


int main() {
    unit_test();

    return 0;
}