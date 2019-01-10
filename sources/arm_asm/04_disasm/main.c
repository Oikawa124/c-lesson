#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "disasm.h"


int print_asm(int word) {
    if (0xe3a01000 == (word & 0xe3a01000)) {

        int _register = (word >> 12) & 0x0000f; // 3桁右にシフトさせてマスク（必要なところを抜き出す）する
        int offset = word & 0x00000fff;

        cl_printf("mov r%x, #0x%x", _register, offset);
        return 1;

    } else if (word == 0xEAFFFFFE) {

        int offset = word & 0x00ffffff;
        int convert_offset = (~offset & 0xffffff) + 0b1;
        // 実際のオフセットは、2bit左にシフトした値
        int actual_offset = convert_offset << 2;

        cl_printf("b [r15, #-0x%x]", actual_offset);
        return 1;

    } else if (word == 0xE59F0030) {
        // ldrの実装
        int offset = word & 0x00000fff;
        int transfer_souse_register = (word >> 12) & 0x0000f;
        int base_register = (word >> 16) & 0x0000f;

        cl_printf("ldr r%x,[r%d, #0x%x]", transfer_souse_register, base_register, offset);
        return 1;

    } else if (word == 0xE5801000 || word == 0xE5802000) {
        // str の実装
        int transfer_souse_register = (word >> 12) & 0x0000f;
        int base_register = (word >> 16) & 0x0000f;

        int offset = word & 0x00000fff;

        cl_printf("str r%x, [r%d, #0x%x]", transfer_souse_register, base_register, offset);
        return 1;

    } else {

        // 無し
    }
    return 0;
}


int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }

static void assert_streq(char *expect, char *actual) {
    assert(streq(expect, actual));
}

static void test_print_asm_immediate68() {
    char *expect = "mov r1, #0x68";
    int input = 0xE3A01068;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}


static void test_print_asm_immediate65() {
    char *expect = "mov r1, #0x65";
    int input = 0xE3A01065;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}


static void test_print_asm_branch() {
    char *expect = "b [r15, #-0x8]";
    int input = 0xEAFFFFFE;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_ldr() {
    char *expect = "ldr r0,[r15, #0x30]";
    int input = 0xE59F0030;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_str1() {
    char *expect = "str r1, [r0, #0x0]";
    int input = 0xE5801000;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_str2() {
    char *expect = "str r2, [r0, #0x0]";
    int input = 0xE5802000;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_not_an_order() {
    int expect = '\0';
    int input = 0x64646464;

    cl_clear_output();
    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 0);
    assert(actual[0] == expect);
}

static void test_print_asm_get_result() {
    char *expect1 = "mov r1, #0x63";
    char *expect2 = "mov r1, #0x65";
    char *expect3 = "mov r1, #0x64";
    int input1 = 0xE3A01063;
    int input2 = 0xE3A01065;
    int input3 = 0xE3A01064;


    print_asm(input1);
    print_asm(input2);
    print_asm(input3);

    char *actual1 = cl_get_result(0);
    char *actual2 = cl_get_result(1);
    char *actual3 = cl_get_result(2);

    assert_streq(expect1, actual1);
    assert_streq(expect2, actual2);
    assert_streq(expect3, actual3);
    cl_clear_output();
}

static void unit_test() {

    cl_enable_buffer_mode();

    // 命令: mov
    test_print_asm_immediate68();
    test_print_asm_immediate65();

    // 命令: b
    test_print_asm_branch();

    // 命令: ldr
    test_print_asm_ldr();

    // 命令: str
    test_print_asm_str1();
    test_print_asm_str2();

    //命令以外
    test_print_asm_not_an_order();

    test_print_asm_get_result();
}


int main() {
    unit_test();

    return 0;
}
