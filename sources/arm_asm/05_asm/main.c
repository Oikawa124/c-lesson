#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#include "asm.h"


// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(char *buf, struct Emitter *emitter) {

    int start = 0;
    unsigned int oneword = 0;

    // 命令切り出し
    struct substring sub_str;
    start = parse_one(buf, start, &sub_str);

//    int mnemonic_symbol = 0;
//
//    if (is_colon(buf, start)) {
//        //　ラベル
//    } else {
//        mnemonic_symbol = to_mnemonic_symbol(&sub_str);
//    }
//
//
//    switch (mnemonic_symbol) {
//        case MOV:
//
//        case STR:
//
//        case LDR:
//
//        case RAW:
//
//        default:
//            printf("Not Implemented");
//    }


    if (strncmp(sub_str.str, "mov", 3) == 0) { // mov
        int reg_1st;
        start = parse_register(buf, start, &reg_1st);

        if (start == PARSE_FAIL) { return start; }

        start = skip_comma(buf, start);

        if (start == PARSE_FAIL) { return start; }

        if (is_register(buf, start)) { // レジスタの場合

            int reg_2nd;
            start = parse_register(buf, start, &reg_2nd);

            if (start == PARSE_FAIL) { return start; }

            oneword = 0xE1A00000;
            oneword += reg_1st << 12;
            oneword += reg_2nd;

        } else { // 即値の場合

            int imm_value;
            start = parse_immediate(buf, start, &imm_value);

            if (start == PARSE_FAIL) { return start; }

            oneword = 0xE3A00000;
            oneword += reg_1st << 12;
            oneword += imm_value;
        }


    } else if ((strncmp(sub_str.str, "ldr", 3) == 0)
                || (strncmp(sub_str.str, "str", 3) == 0)){  // ldr or str

        oneword = 0xE5000000;

        // sourseレジスタ切り出し
        int sourse_reg;

        start = parse_register(buf, start, &sourse_reg);

        if (start == PARSE_FAIL) { return start; }

        start = skip_comma(buf, start);

        if (start == PARSE_FAIL) { return start; }

        start = parse_left_sbracket(buf, start);

        if (start == PARSE_FAIL) { return start; }

        // baseレジスタ切り出し
        int base_reg;

        start = parse_register(buf, start, &base_reg);

        if (start == PARSE_FAIL) { return start; }


        if (is_comma(buf, start)) { // 即値あり　

            start = skip_comma(buf, start);

            int imm_value;
            start = parse_immediate(buf, start, &imm_value);

            if (start == PARSE_FAIL) { return start; }

            start = parse_right_sbracket(buf, start);

            if (start == PARSE_FAIL) { return start; }


            if (imm_value < 0) { // 負のオフセット

                oneword += 0x1 << 20;
                oneword += base_reg << 16;
                oneword += sourse_reg << 12;
                oneword += -1 * imm_value;

            } else { // 正のオフセット

                oneword += 0x9 << 20;
                oneword += base_reg << 16;
                oneword += sourse_reg << 12;
                oneword += imm_value;

            }

        } else { // レジスタのみ

            // ldrかstr判定
            if (strncmp(sub_str.str, "ldr", 3) == 0) {
                oneword += 0x9 << 20; // ldr  ex. ldr r0, [r15]
            } else {
                oneword += 0x8 << 20; // str  ex. str r1, [0]
            }


            start = parse_right_sbracket(buf, start);

            if (start == PARSE_FAIL) { return start; }

            oneword += base_reg << 16;
            oneword += sourse_reg << 12;

        }


    } else if (strncmp(sub_str.str, ".raw", 4) == 0) { //.raw
        unsigned int raw_val;
        start = parse_raw(buf, start, &raw_val);

        if (start == PARSE_FAIL) { return start; }

        oneword = raw_val;
    }

    emit_word(emitter, oneword);

    return 1;
}






/**************** Unit Tests ************************************/


/**************** asm one ************************************/

static void test_asm_one_when_symbol_is_mov_with_reg(){

    // SetUp
    char *input = "mov r1, r2";
    unsigned int expect = 0xE1A01002;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_one_when_symbol_is_mov_with_immediate(){

    // SetUp
    char *input = "mov r1, #0x6c";
    unsigned int expect = 0xE3A0106c;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_one_when_symbol_is_raw_number_only(){

    // SetUp
    char *input = ".raw 0x12345678";
    unsigned int expect = 0x12345678;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_one_when_symbol_is_ldr_with_immediate(){

    // SetUp
    char *input = "ldr r0, [r15, #0x30]";
    unsigned int expect = 0xE59F0030;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_one_when_symbol_is_ldr_with_minus_immediate(){

    // SetUp
    char *input = "ldr r0, [r15, #-0x30]";
    unsigned int expect = 0xE51F0030;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_one_when_symbol_is_ldr_with_no_immediate(){

    // SetUp
    char *input = "ldr r0, [r15]";
    unsigned int expect = 0xE59F0000;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}


static void test_asm_one_when_symbol_is_str(){

    // SetUp
    char *input = "str r1, [r0]";
    unsigned int expect = 0xE5801000;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}


static void unit_tests() {

    // asm one

    //// mov
    test_asm_one_when_symbol_is_mov_with_reg();
    test_asm_one_when_symbol_is_mov_with_immediate();

    //// raw
    test_asm_one_when_symbol_is_raw_number_only();

    //// ldr
    test_asm_one_when_symbol_is_ldr_with_immediate();
    test_asm_one_when_symbol_is_ldr_with_minus_immediate();
    test_asm_one_when_symbol_is_ldr_with_no_immediate();

    //// str
    test_asm_one_when_symbol_is_str();

}

void read_simple_assembly_file(FILE *fp, struct Emitter *emitter){

    cl_getc_set_fp(fp);

    int res;
    int line_num = 0;
    char *buf;

    while (1){

        // 一行読み込み
        int buf_len = 0;
        buf_len = cl_getline(&buf);

        if (buf_len == EOF) { break;}

        res = asm_one(buf, emitter);

        if (res == PARSE_FAIL) {
            printf("PARSE FAIL\n");
            printf("line num: %d", line_num);
            break;
        }

        line_num++;
    }
}

void write_binary_file(struct Emitter *emitter){
    FILE *fp;

    fp = fopen("test.bin", "wb");

    fwrite(emitter->array, sizeof(unsigned int), emitter->pos, fp);

    fclose(fp);
}



int main(int argc, char **argv) {

    set_up();

    unit_tests();

    // アセンブル結果を渡す配列を準備
    struct Emitter emitter;
    initialize_result_arr(&emitter);

    FILE *fp;
    fp = fopen(argv[1], "r");

    if (fp == NULL) { printf("Not exist file");}

    // .ksファイルをアセンブルする
    read_simple_assembly_file(fp, &emitter);

    fclose(fp);

    // バイナリ書き込み
    write_binary_file(&emitter);

    return 0;
}
