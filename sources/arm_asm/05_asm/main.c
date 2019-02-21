#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#include "asm.h"

static int mov_op(char *str, int start, unsigned int *out_oneword){

    int pos = start;

    unsigned int oneword;
    int reg_1st;

    pos = parse_register(str, pos, &reg_1st);

    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);

    if (pos == PARSE_FAIL) { return pos; }

    if (is_register(str, pos)) { // レジスタの場合

        int reg_2nd;
        pos = parse_register(str, pos, &reg_2nd);

        if (pos == PARSE_FAIL) { return pos; }

        oneword = 0xE1A00000;
        oneword += reg_1st << 12;
        oneword += reg_2nd;

    } else { // 即値の場合

        int imm_value;
        pos = parse_immediate(str, pos, &imm_value);

        if (pos == PARSE_FAIL) { return pos; }

        oneword = 0xE3A00000;
        oneword += reg_1st << 12;
        oneword += imm_value;
    }

    *out_oneword = oneword;

    return pos;
}


static int single_data_transfer(char *str, int start, int mnemonic, unsigned int *out_oneword) {
    int pos = start;

    unsigned int oneword = 0xE5000000;

    // sourseレジスタ切り出し
    int sourse_reg;

    pos = parse_register(str, pos, &sourse_reg);

    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);

    if (pos == PARSE_FAIL) { return pos; }

    pos = parse_left_sbracket(str, pos);

    if (pos == PARSE_FAIL) { return pos; }

    // baseレジスタ切り出し
    int base_reg;

    pos = parse_register(str, pos, &base_reg);

    if (pos == PARSE_FAIL) { return pos; }


    if (is_comma(str, pos)) { // 即値あり　

        pos = skip_comma(str, pos);

        int imm_value;
        pos = parse_immediate(str, pos, &imm_value);

        if (pos == PARSE_FAIL) { return pos; }

        pos = parse_right_sbracket(str, pos);

        if (pos == PARSE_FAIL) { return pos; }


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
        if (mnemonic == LDR) {
            oneword += 0x9 << 20; // ldr  ex. ldr r0, [r15]
        } else {
            oneword += 0x8 << 20; // str  ex. str r1, [0]
        }


        pos = parse_right_sbracket(str, pos);

        if (pos == PARSE_FAIL) { return pos; }

        oneword += base_reg << 16;
        oneword += sourse_reg << 12;

    }

    *out_oneword = oneword;

    return pos;
}

static int raw_word(char *str, int start, unsigned int *out_oneword){

    int pos = start;

    unsigned int raw_val;
    start = parse_raw(str, start, &raw_val);

    if (start == PARSE_FAIL) { return start; }

    *out_oneword = raw_val;

    return pos;
}



// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(char *buf, struct Emitter *emitter) {

    int start = 0;
    unsigned int oneword = 0;

    // 命令切り出し
    struct substring sub_str;
    start = parse_one(buf, start, &sub_str);

    int mnemonic_symbol = 0;

    if (is_colon(buf, start)) {
        //　ラベル
    } else {
        mnemonic_symbol = to_mnemonic_symbol(&sub_str);
    }


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


    if (mnemonic_symbol == MOV)
    {
        mov_op(buf, start, &oneword);

    } else if ((mnemonic_symbol == LDR)
                || (mnemonic_symbol == STR))
    {
        single_data_transfer(buf, start, mnemonic_symbol, &oneword);

    } else if (mnemonic_symbol == RAW)
    {
        raw_word(buf, start, &oneword);
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

//    // アセンブル結果を渡す配列を準備
//    struct Emitter emitter;
//    initialize_result_arr(&emitter);
//
//    FILE *fp;
//    fp = fopen(argv[1], "r");
//
//    if (fp == NULL) { printf("Not exist file");}
//
//    // .ksファイルをアセンブルする
//    read_simple_assembly_file(fp, &emitter);
//
//    fclose(fp);
//
//    // バイナリ書き込み
//    write_binary_file(&emitter);

    return 0;
}
