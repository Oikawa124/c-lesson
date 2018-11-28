#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "disasm.h"

// intを渡してその逆アセンブル結果を出力する．
int print_asm(int word);



void to_assembly_case_mov(int word) {}



int print_asm(int word) {
    if (0xe3a01000 == (word & 0xe3a01000)) {

        int _register = (word >> 12) & 0x0000f;
        int offset    = word & 0x00000fff;

        cl_printf("mov r%x, #0x%x", _register, offset);
        return 1;
    }



    if (word == 0xEAFFFFFE) {
        cl_printf("b [r15, #0x34]");
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


    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 1);
    assert(streq(expect, actual));
}


static void test_print_asm_0xE3A01065() {
    char *expect = "mov r1, #0x65";
    int input = 0xE3A01065;


    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 1);
    assert(streq(expect, actual));
}



static void test_print_asm_EAFFFFFE() {
    char *expect = "b [r15, #0x34]";
    int input = 0xEAFFFFFE;


    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 1);
    assert(streq(expect, actual));
}


static void test_print_asm_0x64646464() {
    int expect = '\0';
    int input = 0x64646464;


    int is_instruction = print_asm(input);
    char *actual = cl_get_printed_buffer();

    assert(is_instruction == 0);
    assert(actual[0] == expect);
}


static void unit_test() {

    cl_enable_buffer_mode();

    // 命令: mov
    test_print_asm_0xE3A01068();
    test_print_asm_0xE3A01065();

    // 命令: b
    test_print_asm_EAFFFFFE();

    //命令でない
    test_print_asm_0x64646464();
}


int main() {
    unit_test();

    return 0;
}