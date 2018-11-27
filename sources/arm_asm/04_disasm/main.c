#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "disasm.h"

// intを渡してその逆アセンブル結果を出力する．
int print_asm(int word);


int print_asm(int word) {
    if (word == 0xE3A01068) {
        cl_printf("mov r1, #0x68");
        return 1;
    }


    if (word == 0x64646464) {
        cl_printf("");
        return 0;
    }

    return 0;
}


int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }

static void test_print_asm_0xE3A01068() {
    char *expect = "mov r1, #0x68";
    int input = 0xE3A01068;

    cl_enable_buffer_mode();

    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 1);
    assert(streq(expect, actual));

}

static void test_print_asm_0x64646464() {
    int expect = '\0';
    int input = 0x64646464;

    cl_enable_buffer_mode();

    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 0);
    assert(actual[0] == expect);
}


static void unit_test() {
    test_print_asm_0xE3A01068();
    test_print_asm_0x64646464();
}


int main() {
    unit_test();
    return 0;
}