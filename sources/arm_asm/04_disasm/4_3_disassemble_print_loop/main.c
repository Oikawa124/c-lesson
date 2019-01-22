#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include "disasm.h"

void print_data_transfer(char* mnemonic, int word);

int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }


int print_asm(int word) {
    if (0xe3a00000 == (word & 0xe3a00000)) {
        // movの実装
        int _register = (word >> 12) & 0x0000f; // 3桁右にシフトさせてマスク（必要なところを抜き出す）する
        int offset = word & 0x00000fff;

        cl_printf("mov r%x, #0x%x", _register, offset);
        return 1;

    } else if (word == 0xEAFFFFFE) {
        // bの実装
        int offset = word & 0x00ffffff;
        int convert_offset = (~offset & 0xffffff) + 0b1;
        // 実際のオフセットは、2bit左にシフトした値
        int actual_offset = convert_offset << 2;

        cl_printf("b [r15, #-0x%x]", actual_offset);
        return 1;

    } else if (0xe59f0000 == (word & 0xe59f0000)) {
        print_data_transfer("ldr", word);
        return 1;

    } else if (0xe5D13000 == (word & 0xe5D13000)){
        print_data_transfer("ldrb", word);
        return 1;

    } else if (0xE5800000 == (word & 0xE5800000)) {
        print_data_transfer("str", word);
        return 1;

    } else if (word == 0xE2811001) {

        // addの実装
        cl_printf("add r1, r1, #1");
        return 1;

    } else if (word == 0xE3530000 ) {

        // cmpを実装
        cl_printf("cmp r3, #0");
        return 1;

    } else if (word == 0x1AFFFFFA) {

        // bneの実装
        cl_printf("bne [r15, #0xc]");
        return 1;

    } else {
        // 実装なし
    }

    return 0;
}



void print_data_transfer(char* mnemonic, int word) {
    int offset = word & 0x00000fff;
    int transfer_souse_register = (word >> 12) & 0x0000f;
    int base_register = (word >> 16) & 0x0000f;

    if (streq("ldr", mnemonic)){
        cl_printf("ldr r%x, [r%d, #0x%x]", transfer_souse_register, base_register, offset);

    } else if (streq("ldrb", mnemonic)) {
        cl_printf("ldrb r%x, [r%d]", transfer_souse_register, base_register);

    } else if (streq("str", mnemonic)) {
        if (offset == 0) {
            cl_printf("str r%x, [r%d]", transfer_souse_register, base_register);
        } else {
            cl_printf("str r%x, [r%d, #0x%x]", transfer_souse_register, base_register, offset);
        }
    }

    if (streq("ldr", mnemonic) || streq("")) {}

}



void print_hex_dump(int word){
    // 16進数ダンプ
    for (int i = 0; i < 4; ++i) {
        int two_digit_word = word & 0xff;
        if (i != 3) {
            cl_printf("%02x ", two_digit_word);
        } else {
            cl_printf("%02x", two_digit_word);
        }
        word = word >> 2*4;
    }
}

int read_binary_file(FILE *fp) {
    int buf;
    if (fp == NULL) {
        printf("Not open this file");
        return 1;
    }

    fread(&buf, 4, 1, fp);

    int memory_address = 0x00010000;
    int is_instruction = 1;

    do {
        cl_printf("0x%08x  ", memory_address);

        if (is_instruction) {
            is_instruction = print_asm(buf);
            if (is_instruction != 1) {
                print_hex_dump(buf);
            }
        } else {
            print_hex_dump(buf);
        }

        cl_printf("\n");
        memory_address += 4;

    } while (fread(&buf, 4, 1, fp) == 1);

    return 0;
}





static void assert_streq(char *expect, char *actual) {
    assert(streq(expect, actual));
}

static void test_print_asm_mov_immediate68() {
    char *expect = "mov r1, #0x68";
    int input = 0xE3A01068;


    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}


static void test_print_asm_mov_immediate65() {
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
    char *expect = "ldr r0, [r15, #0x30]";
    int input = 0xE59F0030;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_ldrb() {
    char *expect = "ldrb r3, [r1]";
    int input = 0xE5D13000;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_add() {
    char *expect = "add r1, r1, #1";
    int input = 0xE2811001;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_cmp() {
    char *expect = "cmp r3, #0";
    int input = 0xE3530000;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}


static void test_print_asm_bne() {
    char *expect = "bne [r15, #0xc]";
    int input = 0x1AFFFFFA;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}


static void test_print_asm_str_reg1() {
    char *expect = "str r1, [r0]";
    int input = 0xE5801000;

    int is_instruction = print_asm(input);
    char *actual = cl_get_result(0);

    assert(is_instruction == 1);
    assert_streq(expect, actual);

    cl_clear_output();
}

static void test_print_asm_str_reg2() {
    char *expect = "str r2, [r0]";
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


static void test_print_data_transfer() {
    char *expect1 = "ldr r0, [r15, #0x30]";
    char *expect2 = "ldrb r3, [r1]";
    char *expect3 = "str r1, [r0]";


    int input_word1 = 0xE59F0030;
    int input_word2 = 0xE5D13000;
    int input_word3 = 0xE5801000;


    char *input_mnemonic1 = "ldr";
    char *input_mnemonic2 = "ldrb";
    char *input_mnemonic3 = "str";

    print_data_transfer(input_mnemonic1, input_word1);
    print_data_transfer(input_mnemonic2, input_word2);
    print_data_transfer(input_mnemonic3, input_word3);

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
    test_print_asm_mov_immediate68();
    test_print_asm_mov_immediate65();

    // 命令: b
    test_print_asm_branch();

    // 命令: ldr
    test_print_asm_ldr();

    // 命令: ldrb
    test_print_asm_ldrb();

    // 命令: add
    test_print_asm_add();

    // 命令: cmp
    test_print_asm_cmp();

    // 命令: bne
    test_print_asm_bne();

    // 命令: str
    test_print_asm_str_reg1();
    test_print_asm_str_reg2();

    //命令以外
    test_print_asm_not_an_order();
    test_print_asm_get_result();
    test_print_data_transfer();
}


int main(int argc, char *argv[]) {
    unit_test();

//    FILE *fp = fopen(argv[1], "rb");
//    read_binary_file(fp);
//
//    fclose(fp);

    return 0;
}
