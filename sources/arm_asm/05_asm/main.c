#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

#include "asm.h"

static int asm_mov_op(char *str, int start, struct Emitter *emitter){

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

    emit_word(emitter, oneword);
    return pos;
}


static int asm_add_op(char *str, int start, struct Emitter *emitter){

    int pos = start;

    unsigned int oneword = 0xE2800000;

    int reg_1st;
    pos = parse_register(str, pos, &reg_1st);
    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    int destination_reg;
    pos = parse_register(str, pos, &destination_reg);
    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    int imm_value;
    pos = parse_immediate(str, pos, &imm_value);
    if (pos == PARSE_FAIL) { return pos; }


    oneword += reg_1st << 16;
    oneword += destination_reg << 12;
    oneword += imm_value;

    emit_word(emitter, oneword);

    return pos;
}

static int asm_cmp_op(char *str, int start, struct Emitter *emitter){

    int pos = start;

    unsigned int oneword = 0xE3500000;

    int reg_1st;
    pos = parse_register(str, pos, &reg_1st);
    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    int imm_value;
    pos = parse_immediate(str, pos, &imm_value);
    if (pos == PARSE_FAIL) { return pos; }


    oneword += reg_1st << 16;
    oneword += imm_value;

    emit_word(emitter, oneword);

    return pos;
}


static int asm_ldrb_op(char *str, int start, struct Emitter *emitter){

    int pos = start;

    unsigned int oneword = 0xE5D00000;

    int destination_reg;
    pos = parse_register(str, pos, &destination_reg);
    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    pos = parse_left_sbracket(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    int base_reg;
    pos = parse_register(str, pos, &base_reg);
    if (pos == PARSE_FAIL) { return pos; }

    pos = parse_right_sbracket(str, pos);
    if (pos == PARSE_FAIL) { return pos; }


    oneword += destination_reg << 12;
    oneword += base_reg << 16;

    emit_word(emitter, oneword);

    return pos;
}


static int asm_ldr_relative_offset(char *str, int pos,
                                   int base_reg, int sourse_reg,
                                   struct Emitter *emitter) {

    unsigned int oneword = 0xE5000000;
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

    emit_word(emitter, oneword);
    return pos;
}


static int asm_ldr_or_str_register_only(char *str, int pos, int mnemonic,
                                        int base_reg, int sourse_reg,
                                        struct Emitter *emitter) {

    unsigned int oneword = 0xE5000000;

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


    emit_word(emitter, oneword);
    return pos;
}


static int asm_ldr_label(char *str, int pos,
                         int sourse_reg,
                         struct Emitter *emitter) {

    unsigned int oneword = 0xE5900000;
    oneword += 0xF << 16;
    oneword += sourse_reg << 12;

    emit_word(emitter, oneword);


    pos = skip_equal_sign(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    struct substring sub_str;
    pos = parse_one(str, pos, &sub_str); // ラベルをパース
    if (pos == PARSE_FAIL) { return pos; }

    int label_symbol = to_label_symbol(&sub_str);

    // ラベルのアドレスの解決に必要なものを覚えておく
    int mnemonic = LDR;
    unsigned int memory_address = get_last_memory_address(emitter);

    // emitter.posは次のバイナリを入れる場所を示しているため、-1する。
    add_unresolve_list(mnemonic, emitter->pos-1, memory_address, label_symbol);

    return pos;
}

static int asm_single_data_transfer(char *str, int start, int mnemonic,
                                    struct Emitter *emitter) {
    int pos = start;

    // sourseレジスタ切り出し
    int sourse_reg;

    pos = parse_register(str, pos, &sourse_reg);
    if (pos == PARSE_FAIL) { return pos; }

    pos = skip_comma(str, pos);
    if (pos == PARSE_FAIL) { return pos; }


    // ldr rX, =address　の場合
    if (is_equal_sign(str, pos)) {
        asm_ldr_label(str, pos, sourse_reg, emitter);
        return pos;
    }

    pos = parse_left_sbracket(str, pos);
    if (pos == PARSE_FAIL) { return pos; }

    // baseレジスタ切り出し
    int base_reg;
    pos = parse_register(str, pos, &base_reg);
    if (pos == PARSE_FAIL) { return pos; }


    if (is_comma(str, pos)) { // 即値あり　ex. ldr r0, [r15, #0x03]
        pos = asm_ldr_relative_offset(str, pos, base_reg, sourse_reg, emitter);
        if (pos == PARSE_FAIL) { return pos; }
        return pos;

    } else { // レジスタのみ  ex. ldr r0, [r15], str ...
        pos = asm_ldr_or_str_register_only(str, pos, mnemonic, base_reg, sourse_reg, emitter);
        if (pos == PARSE_FAIL) { return pos; }
        return pos;
    }
}



void resolve_address(struct Emitter *emitter){
    // ここでラベルのアドレスを解決する。

    unresolve_list *node = unresolve_list_head;

    // 命令: b, ldrに対応
    while (node != NULL) {

        unsigned int label_address;
        dict_get(node->label_symbol, &label_address);

        if (node->mnemonic == LDR) { //ldrの場合　一番後ろにラベルの位置へのアドレスを追加する
            emit_word(emitter, label_address);
            unsigned int memory_address = get_last_memory_address(emitter);

            label_address = memory_address;
        }

        int offset;
        int is_minus_offset = 0;

        // r15(pc)は8個先を示しているため、-8する
        if (node->op_address < label_address) {
            offset = label_address - node->op_address - 8;
        } else {
            offset = node->op_address - label_address - 8;
            is_minus_offset = 1;
        }

        if (is_minus_offset) {
            offset = (~(-1*offset)) + 1; //2の補数表現
        }

        if ((node->mnemonic == B) || node->mnemonic == BNE) {
            offset = (offset >> 2) & 0xFFFFFF; // bの場合、右に2bitシフトして24bit分マスク
        }

        emitter->array[node->emit_arr_pos] += offset;
        node = node->next;
    }
}

static int asm_branch(char *str, int start, int mnemonic, struct Emitter *emitter){
    int pos = start;

    unsigned int oneword = 0;
    if (mnemonic == B) {
        oneword = 0xEA000000;
    } else if(mnemonic == BNE){
        oneword = 0x1A000000;
    } else {
        printf("Not Implemented");
    }

    emit_word(emitter, oneword);

    struct substring sub_str;
    pos = parse_one(str, pos, &sub_str); // ラベルをパース
    if (pos == PARSE_FAIL) { return pos; }

    int label_symbol = to_label_symbol(&sub_str);

    // ラベルのアドレスの解決に必要なものを覚えておく
    unsigned int memory_address = get_last_memory_address(emitter);

    // emitter.posは次のバイナリを入れる場所を示しているため、-1する。
    add_unresolve_list(mnemonic, emitter->pos-1, memory_address, label_symbol);

    return pos;
};


static int asm_raw_op(char *str, int start, struct Emitter *emitter){

    int pos = start;

    pos = skip_space(str, pos);

    if (str[pos] == '"') { // 文字列パース
        char *str_val;
        pos = parse_string(str, pos, &str_val);
        if (pos == PARSE_FAIL) { return pos; }

        //emitterに4byteずづ詰める
        int i = 0;
        int str_pos = 0;
        unsigned int oneword = 0;

        while (str_val[str_pos] != '\0') {

            oneword += (int) str_val[str_pos] << 8 * i;
            str_pos++;
            i++;

            if (i == 4) {
                emit_word(emitter, oneword);
                oneword = 0;
                i = 0;
            }
        }

        if (i != 0) {
            emit_word(emitter, oneword);
        }

    } else { // 数字パース
        unsigned int raw_num_val;
        pos = parse_raw_value(str, pos, &raw_num_val);
        if (pos == PARSE_FAIL) { return pos; }

        emit_word(emitter, raw_num_val);
    }
    return pos;
}



// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(char *buf, struct Emitter *emitter) {

    int res = 1;
    int start = 0;

    // 命令切り出し
    struct substring sub_str;
    start = parse_one(buf, start, &sub_str);

    int mnemonic_symbol = 0;
    int label_symbol = 0;


    if (is_colon(buf, start)) { // ラベル
        label_symbol = to_label_symbol(&sub_str);

        // ラベルはemitされないため、次にemittされるバイナリのメモリ上のアドレスになる。
        unsigned int memory_address = get_last_memory_address(emitter) + 4;
        dict_put(label_symbol, memory_address);
        return res;

    } else { // シンボル
        mnemonic_symbol = to_mnemonic_symbol(&sub_str);
    }


    // mnemonicで分岐
    if (mnemonic_symbol == MOV) {

        res = asm_mov_op(buf, start, emitter);

    } else if ((mnemonic_symbol == LDR)
               || (mnemonic_symbol == STR)) {

        res = asm_single_data_transfer(buf, start, mnemonic_symbol, emitter);

    } else if ((mnemonic_symbol == B)
               || (mnemonic_symbol == BNE)) {

        res = asm_branch(buf, start, mnemonic_symbol, emitter);

    } else if (mnemonic_symbol == RAW) {

        res = asm_raw_op(buf, start, emitter);

    } else if (mnemonic_symbol == ADD) {

        res = asm_add_op(buf, start, emitter);

    } else if (mnemonic_symbol == CMP) {

        res = asm_cmp_op(buf, start, emitter);

    } else if (mnemonic_symbol == LDRB) {

        res = asm_ldrb_op(buf, start, emitter);

    } else {
        printf("Not Implemented");
    }

    return res;
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

    // TearDown
    initialize_when_test();
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

    // TearDown
    initialize_when_test();
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

    // TearDown
    initialize_when_test();
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

    // TearDown
    initialize_when_test();
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

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_is_ldr_with_label(){
    // SetUp
    char *input1 = "ldr r1, =msg";
    char *input2 = "msg:";
    char *input3 = ".raw \"hello\"";

    unsigned int expect1 = 0xe59f1004; //0x1000
    unsigned int expect2 = 0x6c6c6568; //0x1004
    unsigned int expect3 = 0x0000006f; //0x100c
    unsigned int expect4 = 0x00010004; //0x1010

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input1, &emitter);
    asm_one(input2, &emitter);
    asm_one(input3, &emitter);
    resolve_address(&emitter);

    // Verify
    assert(expect1 == emitter.array[0]);
    assert(expect2 == emitter.array[1]);
    assert(expect3 == emitter.array[2]);
    assert(expect4 == emitter.array[3]);

    // TearDown
    initialize_when_test();
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

static void test_asm_one_when_is_b(){
    // SetUp
    char *input1 = "loop:";
    char *input2 = "b loop";

    unsigned int expect = 0xEAFFFFFE;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input1, &emitter);
    asm_one(input2, &emitter);
    resolve_address(&emitter);

    // Verify
    assert(expect == emitter.array[0]);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_is_b_with_after_label(){
    // SetUp
    char *input1 = "b loop";
    char *input2 = "loop:";

    unsigned int expect = 0xEAFFFFFF;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input1, &emitter);
    asm_one(input2, &emitter);
    resolve_address(&emitter);

    // Verify
    assert(expect == emitter.array[0]);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_is_b_with_far_after_label(){
    // SetUp
    char *input1 = "b loop";
    char *input2 = "mov r1, r2";
    char *input3 = "mov r1, r2";
    char *input4 = "loop:";

    unsigned int expect = 0xEA000001;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input1, &emitter);
    asm_one(input2, &emitter);
    asm_one(input3, &emitter);
    asm_one(input4, &emitter);

    resolve_address(&emitter);

    // Verify
    assert(expect == emitter.array[0]);

    // TearDown
    initialize_when_test();
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

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_raw_string(){

    // SetUp
    char *input = ".raw \"test\"";
    unsigned int expect = 0x74736574;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_when_test();
;
}

static void test_asm_one_when_symbol_is_raw_string_with_new_line(){

    // SetUp
    char *input = ".raw \"Hello World\\n\"";
    unsigned int expect1 = 0x6c6c6548;
    unsigned int expect2 = 0x6f57206f;
    unsigned int expect3 = 0x0a646c72;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual1 = emitter.array[0];
    unsigned int actual2 = emitter.array[1];
    unsigned int actual3 = emitter.array[2];

    // Verify
    assert(expect1 == actual1);
    assert(expect2 == actual2);
    assert(expect3 == actual3);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_raw_string_with_double_quart(){

    // SetUp
    char *input = ".raw \"escape1 \\\" end\"";
    unsigned int expect1 = 0x61637365;
    unsigned int expect2 = 0x20316570;
    unsigned int expect3 = 0x6e652022;
    unsigned int expect4 = 0x00000064;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual1 = emitter.array[0];
    unsigned int actual2 = emitter.array[1];
    unsigned int actual3 = emitter.array[2];
    unsigned int actual4 = emitter.array[3];

    // Verify
    assert(expect1 == actual1);
    assert(expect2 == actual2);
    assert(expect3 == actual3);
    assert(expect4 == actual4);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_raw_string_with_escape(){

    // SetUp
    char *input = ".raw \"escape2 \\\\ end\"";
    unsigned int expect1 = 0x61637365;
    unsigned int expect2 = 0x20326570;
    unsigned int expect3 = 0x6e65205c;
    unsigned int expect4 = 0x00000064;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual1 = emitter.array[0];
    unsigned int actual2 = emitter.array[1];
    unsigned int actual3 = emitter.array[2];
    unsigned int actual4 = emitter.array[3];

    // Verify
    assert(expect1 == actual1);
    assert(expect2 == actual2);
    assert(expect3 == actual3);
    assert(expect4 == actual4);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_raw_string_with_escape_2(){

    // SetUp
    char *input = ".raw \"escape3 \\\\\"";
    unsigned int expect1 = 0x61637365;
    unsigned int expect2 = 0x20336570;
    unsigned int expect3 = 0x0000005c;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual1 = emitter.array[0];
    unsigned int actual2 = emitter.array[1];
    unsigned int actual3 = emitter.array[2];

    // Verify
    assert(expect1 == actual1);
    assert(expect2 == actual2);
    assert(expect3 == actual3);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_raw_string_with_escape_and_d_quart(){

    // SetUp
    char *input = ".raw \"escape4 \\\\\\\" end\"";
    unsigned int expect1 = 0x61637365;
    unsigned int expect2 = 0x20346570;
    unsigned int expect3 = 0x6520225c;
    unsigned int expect4 = 0x0000646e;


    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual1 = emitter.array[0];
    unsigned int actual2 = emitter.array[1];
    unsigned int actual3 = emitter.array[2];
    unsigned int actual4 = emitter.array[3];

    // Verify
    assert(expect1 == actual1);
    assert(expect2 == actual2);
    assert(expect3 == actual3);
    assert(expect4 == actual4);

    // TearDown
    initialize_when_test();
}



static void test_asm_one_when_symbol_is_add(){

    // SetUp
    char *input = "add r1, r1, #0x1";
    unsigned int expect = 0xE2811001;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_when_test();
}


static void test_asm_one_when_symbol_is_cmp(){

    // SetUp
    char *input = "cmp r3, #0x0";
    unsigned int expect = 0xE3530000;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_when_test();
}


static void test_asm_one_when_symbol_is_ldrb(){

    // SetUp
    char *input = "ldrb r3, [r1]";
    unsigned int expect = 0xE5d13000;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_when_test();
}

static void test_asm_one_when_symbol_is_bne(){

    // SetUp
    char *input1 = "loop:";
    char *input2 = "bne loop";

    unsigned int expect = 0x1AFFFFFE ;

    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // Exercise
    asm_one(input1, &emitter);
    asm_one(input2, &emitter);
    resolve_address(&emitter);

    // Verify
    assert(expect == emitter.array[0]);

    // TearDown
    initialize_when_test();
}



static void unit_tests() {

    // asm one

//    //// mov
//    test_asm_one_when_symbol_is_mov_with_reg();
//    test_asm_one_when_symbol_is_mov_with_immediate();
//
//    //// ldr
//    test_asm_one_when_symbol_is_ldr_with_immediate();
//    test_asm_one_when_symbol_is_ldr_with_minus_immediate();
//    test_asm_one_when_symbol_is_ldr_with_no_immediate();
//    test_asm_one_when_is_ldr_with_label();
//
//    //// str
//    test_asm_one_when_symbol_is_str();
//
//    // b
//    test_asm_one_when_is_b();
//    test_asm_one_when_is_b_with_after_label();
//    test_asm_one_when_is_b_with_far_after_label();
//
//
//    //// raw
//    test_asm_one_when_symbol_is_raw_number_only();
//    test_asm_one_when_symbol_is_raw_string();
//    test_asm_one_when_symbol_is_raw_string_with_new_line();
//    test_asm_one_when_symbol_is_raw_string_with_double_quart();
//    test_asm_one_when_symbol_is_raw_string_with_escape();
//    test_asm_one_when_symbol_is_raw_string_with_escape_2();
//    test_asm_one_when_symbol_is_raw_string_with_escape_and_d_quart();
//
//    //// add
//    test_asm_one_when_symbol_is_add();
//
//    //// cmp
//    test_asm_one_when_symbol_is_cmp();
//
//    //// ldrb
//    test_asm_one_when_symbol_is_ldrb();

    /// bne
    test_asm_one_when_symbol_is_bne();

}


void read_simple_assembly_file(FILE *fp, struct Emitter *emitter){

    cl_getc_set_fp(fp);

    int res;
    char *buf;

    while (1){

        // 一行読み込み
        int buf_len = 0;
        buf_len = cl_getline(&buf);

        if (buf_len == EOF) {
            resolve_address(emitter);
            break;
        }

        res = asm_one(buf, emitter);

        if (res == PARSE_FAIL) {
            printf("PARSE FAIL\n");
            printf("line num: %d", memory_address);
            break;
        }
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
