#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

#include "asm.h"


#define PARSE_FAIL -1

// 配列関係
static unsigned int g_asm_result[1000];

struct Emitter {
    unsigned int *array;
    int pos;
};

void emit_word(struct Emitter* emitter, unsigned int oneword){
    emitter->array[emitter->pos] = oneword;
    emitter->pos++;
}



// 文字列切り出し
struct substring {
    char *str;
    int len;
};


// トークン切り出し
int parse_one(char *str, int start, struct substring* out_sub_str){

    int pos = start;
    int ch;

    // 空白スキップ
    while (str[pos] == ' ') { pos++; }


    int sub_str_start_pos = pos;

    // ":"または，","の場合
    if ((ch = str[pos]) == ':'
         || ch == ',')
    {
        out_sub_str->str = &str[sub_str_start_pos];
        out_sub_str->len = 1;
        pos++;

        return pos;
    }

    // シンボル取得
    while ((ch = str[pos]) !='\0'
            && ch != ' '
            && ch != ':'
            && ch != ',')
    {
        pos++;
    }

    out_sub_str->str = &str[sub_str_start_pos];
    out_sub_str->len = pos - sub_str_start_pos;

    return pos;
}


int parse_register(char *str, int start, int *out_register){
    int pos = start;
    int reg_num = 0;

    // 空白スキップ
    while (str[pos] == ' ') {
        pos++;
    }

    // "r"読み飛ばし
    if (str[pos] != 'r') { return PARSE_FAIL;}
    pos++;


    // 数字取得
    while(isdigit(str[pos])){
        reg_num = reg_num * 10 + (str[pos] - '0');
        pos++;
    }

    if ((0 <= reg_num && reg_num <= 15)) {
        *out_register = reg_num;
        return pos;
    } else {
        return PARSE_FAIL;
    }
}

int parse_immediate(char *str, int start, int *out_imm_value){
    int pos = start;
    int is_digit = 0;

    // 空白スキップ
    while (str[pos] == ' ') { pos++; }

    // "#"読み飛ばし
    if (str[pos] != '#') { return PARSE_FAIL; }
    pos++;

    // "0"読み飛ばし
    if (str[pos] != '0') { return PARSE_FAIL;}
    pos++;

    // "x"読み飛ばし
    if (str[pos] != 'x') { return PARSE_FAIL;}
    pos++;


    // 文字列の中の数字部分の範囲を取得
    char hex_num[5];
    struct substring sub_hex = {.str=&str[pos], .len=0};

    while(isxdigit(str[pos])){
        pos++;
        sub_hex.len++;

        is_digit = 1;
    }

    if (is_digit) {
        // 文字列の数字部分切り出し
        strncpy(hex_num, sub_hex.str, sub_hex.len);

        // 文字列を16進数変換
        char *endp;
        *out_imm_value = strtol(hex_num, &endp, 16);

        return pos;
    } else {
        return PARSE_FAIL;
    }

}



int skip_comma(char *str, int start){
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

    // コンマ読み飛ばし
    if (str[pos] != ',') { return PARSE_FAIL; }
    pos++;

    return pos;
}

int is_register(char *str, int start) {
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

    if (str[pos] == 'r') {
        return 1;
    }
    else {
        return 0;
    }
}


// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(char *buf, struct Emitter *emitter){

    int start=0;

    // 命令切り出し
    struct substring sub_str;
    start = parse_one(buf, start, &sub_str);


    unsigned int oneword = 0;

    if (strncmp(sub_str.str,"mov", 3) == 0) {

        // 1stレジスタ切り出し
        int reg_1st;

        start = parse_register(buf, start, &reg_1st);

        if (start == PARSE_FAIL) { return start; }

        start = skip_comma(buf, start);

        if (start == PARSE_FAIL) { return start; }


        if (is_register(buf, start)) { // レジスタの場合
            int reg_2nd;
            start = parse_register(buf, start, &reg_2nd);

            if (start == PARSE_FAIL) { return start; }

            oneword += 0xE1A00000;
            oneword += reg_1st << 12 ;
            oneword += reg_2nd;

        } else { // 即値の場合
            int imm_value;
            start = parse_immediate(buf, start, &imm_value);

            if (start == PARSE_FAIL) { return start; }

            oneword += 0xE3A00000;
            oneword += reg_1st << 12 ;
            oneword += imm_value;
        }

        emit_word(emitter, oneword);
    }

    return 1;
}






/**************** Unit Tests ************************************************/

int strneq(char *s1, char *s2, int len) { return 0 == strncmp(s1, s2, len); }

void assert_substring_eq(char *expect, struct substring* actual){
    assert(strneq(expect, actual->str, actual->len));
}


static void test_parse_one_when_call_once(){

    // SetUp
    char *input = "mov r1, r2";
    int start = 0;

    char *expect = "mov";

    struct substring actual1;

    // Exercise
    start = parse_one(input, start, &actual1);

    // Verify
    assert_substring_eq(expect, &actual1);
}


static void test_parse_one_when_everything_parse() {

    // SetUp
    char *input = "loop: mov r1, r2";
    int start = 0;

    char *expect1 = "loop";
    char *expect2 = ":";
    char *expect3 = "mov";
    char *expect4 = "r1";
    char *expect5 = ",";
    char *expect6 = "r2";

    struct substring actual1;
    struct substring actual2;
    struct substring actual3;
    struct substring actual4;
    struct substring actual5;
    struct substring actual6;

    // Exercise
    start = parse_one(input,start, &actual1);
    start = parse_one(input,start, &actual2);
    start = parse_one(input,start, &actual3);
    start = parse_one(input,start, &actual4);
    start = parse_one(input,start, &actual5);
    start = parse_one(input,start, &actual6);

    // Verify
    assert_substring_eq(expect1, &actual1);
    assert_substring_eq(expect2, &actual2);
    assert_substring_eq(expect3, &actual3);
    assert_substring_eq(expect4, &actual4);
    assert_substring_eq(expect5, &actual5);
    assert_substring_eq(expect6, &actual6);
}

static void test_parse_one_when_parse_one_colon(){

    // SetUp
    char *input = " :";
    int start = 0;

    char *expect = ":";

    struct substring actual;

    // Exercise
    start = parse_one(input, start, &actual);

    // Verify
    assert_substring_eq(expect, &actual);
}

static void test_parse_register_when_call_once() {

    // SetUp
    char *input = "r1, r2";
    int start = 0;

    int expect_reg_1 = 1;

    int actual_reg_1;

    // Exercise
    start = parse_register(input,start, &actual_reg_1);

    // Verify
    assert(expect_reg_1 == actual_reg_1);
}

static void test_parse_register_when_parse_two_registers() {

    // SetUp
    char *input = " r1, r2";
    int start = 0;

    int expect_reg_1 = 1;
    int expect_reg_2 = 2;

    int actual_reg_1, actual_reg_2;

    // Exercise
    start = parse_register(input,start, &actual_reg_1);

    start = skip_comma(input, start);

    start = parse_register(input, start, &actual_reg_2);

    // Verify
    assert(expect_reg_1 == actual_reg_1);
    assert(expect_reg_2 == actual_reg_2);
}


static void test_parse_register_when_parse_fail() {

    // SetUp
    char *input = "d1";
    int start = 0;

    int expect_res = PARSE_FAIL;

    int actual_reg_1;
    int actual_res;

    // Exercise
    actual_res = parse_register(input,start, &actual_reg_1);

    // Verify
    assert(expect_res == actual_res);
}


static void test_parse_immediate_when_call_once() {

    // SetUp
    char *input = "#0x01";
    int start = 0;

    int expect_imm_value = 1;

    int actual_imm_value;

    // Exercise
    start = parse_immediate(input, start, &actual_imm_value);

    // Verify
    assert(expect_imm_value == actual_imm_value);
}


static void test_parse_immediate_when_call_once_with_leading_space() {

    // SetUp
    char *input = "  #0x05";
    int start = 0;

    int expect_imm_value = 5;

    int actual_imm_value;

    // Exercise
    start = parse_immediate(input, start, &actual_imm_value);

    // Verify
    assert(expect_imm_value == actual_imm_value);
}

void test_parse_immediate_when_hexadecimal(){

    // SetUp
    char *input = "#0x0f";
    int start = 0;

    int expect_imm_value = 0xf;

    int actual_imm_value;

    // Exercise
    start = parse_immediate(input, start, &actual_imm_value);

    // Verify
    assert(expect_imm_value == actual_imm_value);
}

void test_parse_immediate_when_imm0x64(){

    // SetUp
    char *input = "#0x64";
    int start = 0;

    int expect_imm_value = 0x64;

    int actual_imm_value;

    // Exercise
    start = parse_immediate(input, start, &actual_imm_value);

    // Verify
    assert(expect_imm_value == actual_imm_value);
}









static void test_asm_when_symbol_is_mov_with_reg(){

    // SetUp
    char *input = "mov r1, r2";
    unsigned int expect = 0xE1A01002;


    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void test_asm_when_symbol_is_mov_with_immediate(){

    // SetUp
    char *input = "mov r1, #0x6c";
    unsigned int expect = 0xE3A0106c;


    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

static void unit_tests() {

    // parse_one
    test_parse_one_when_call_once();
    test_parse_one_when_everything_parse();
    test_parse_one_when_parse_one_colon();

    // parse_register
    test_parse_register_when_call_once();
    test_parse_register_when_parse_two_registers();
    test_parse_register_when_parse_fail();

    // parse_immediate
    test_parse_immediate_when_call_once();
    test_parse_immediate_when_call_once_with_leading_space();
    test_parse_immediate_when_hexadecimal();
    test_parse_immediate_when_imm0x64();

    // asm
    test_asm_when_symbol_is_mov_with_reg();
    test_asm_when_symbol_is_mov_with_immediate();

}


int main() {

    unit_tests();

    // 結果を渡す配列を準備

    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

    FILE *fp;
    fp = fopen("mov_op.ks", "r");

    if (fp == NULL) { printf("Not exist file");}
    cl_getc_set_fp(fp);


    int res;
    int line_num = 0;
    char *buf;

    while (1){

        // 一行読み込み
        int buf_len = 0;
        buf_len = cl_getline(&buf);

        if (buf_len == EOF) { break;}

        res = asm_one(buf, &emitter);

        if (res == PARSE_FAIL) {
            printf("PARSE FAIL");
            break;
        }

        line_num++;
    }

    fclose(fp);

    return 0;
}

