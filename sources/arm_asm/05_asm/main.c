#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#include "asm.h"


#define PARSE_FAIL -2

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

int single_atoi_hex(char *str) {

    if (('0' <= *str) && (*str <= '9')) {
        return *str - 48;
    } else if (('a' <= *str) && (*str <= 'f')) {
        return *str - 87;
    } else if (('A' <= *str) && (*str <= 'F')) {
        return *str - 55;
    } else {
        return PARSE_FAIL;
    }
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
    int is_negative_num = 0;

    // 空白スキップ
    while (str[pos] == ' ') { pos++; }

    // "#"読み飛ばし
    if (str[pos] != '#') { return PARSE_FAIL; }
    pos++;

    // "-"判定
    if (str[pos] == '-') {
        is_negative_num = 1;
        pos++;
    }

    // "0"読み飛ばし
    if (str[pos] != '0') { return PARSE_FAIL;}
    pos++;

    // "x"読み飛ばし
    if (str[pos] != 'x') { return PARSE_FAIL;}
    pos++;


    // 文字列の中の数字部分の範囲を取得
    unsigned int hex_num = 0;

    while(isxdigit(str[pos])){

        if (hex_num != 0) {
            hex_num = hex_num << 4;
        }

        hex_num += single_atoi_hex(&str[pos]);

        if (hex_num == PARSE_FAIL) { return PARSE_FAIL;}

        pos++;
    }

    if (is_negative_num) {
        hex_num = -1*hex_num;
    }

    *out_imm_value = hex_num;

    return pos;
}


int parse_raw(char *str, int start, unsigned int *out_raw_value){
    int pos = start;

    // 空白スキップ
    while (str[pos] == ' ') { pos++; }

    // "0"読み飛ばし
    if (str[pos] != '0') { return PARSE_FAIL;}
    pos++;

    // "x"読み飛ばし
    if (str[pos] != 'x') { return PARSE_FAIL;}
    pos++;


    // 文字列の中の数字部分の範囲を取得
    unsigned int hex_num = 0;

    while(isxdigit(str[pos])){

        if (hex_num != 0) {
            hex_num = hex_num << 4;
        }

        hex_num += single_atoi_hex(&str[pos]);

        if (hex_num == PARSE_FAIL) {
            return PARSE_FAIL;
        }

        pos++;
    }

    *out_raw_value = hex_num;
    return pos;
}

int parse_left_sbracket(char *str, int start){
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

    // コンマ読み飛ばし
    if (str[pos] != '[') { return PARSE_FAIL; }
    pos++;

    return pos;
}

int parse_right_sbracket(char *str, int start){
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

    // コンマ読み飛ばし
    if (str[pos] != ']') { return PARSE_FAIL; }
    pos++;

    return pos;
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

int is_comma(char *str, int start) {
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

    if (str[pos] == ',') {
        return 1;
    }
    else {
        return 0;
    }
}



// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(char *buf, struct Emitter *emitter) {

    int start = 0;

    // 命令切り出し
    struct substring sub_str;
    start = parse_one(buf, start, &sub_str);


    unsigned int oneword = 0;

    if (strncmp(sub_str.str, "mov", 3) == 0) { // mov命令

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
                || (strncmp(sub_str.str, "str", 3) == 0)) {

        oneword = 0xE0000000;

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


        // 即値とレジスタ、または、レジスタのみ
        if (is_comma(buf, start)) { // 即値
            oneword += 0x5 << 24;

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

            } else {

                oneword += 0x9 << 20;
                oneword += base_reg << 16;
                oneword += sourse_reg << 12;
                oneword += imm_value;

            }

        } else { // レジスタのみ
            oneword += 0x5 << 24;

            if (strncmp(sub_str.str, "ldr", 3) == 0) {
                oneword += 0x9 << 20;
            } else { // "strの場合"
                oneword += 0x8 << 20;
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

int strneq(char *s1, char *s2, int len) { return 0 == strncmp(s1, s2, len); }

void assert_substring_eq(char *expect, struct substring* actual){
    assert(strneq(expect, actual->str, actual->len));
}

/**************** parse one ******************************/

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

static void test_parse_one_with_raw(){

    // SetUp
    char *input = ".raw 0x12345678";
    int start = 0;

    char *expect = ".raw";

    struct substring actual;

    // Exercise
    start = parse_one(input, start, &actual);

    // Verify
    assert_substring_eq(expect, &actual);
}



/**************** parse register ***************************/

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


/**************** parse immediate ***************************/

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


static void test_parse_immediate_with_leading_space() {

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

void test_parse_immediate_with_hexadecimal(){

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

static void test_parse_immediate_with_imm0x64(){

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

static void test_parse_immediate_with_imm0xFA(){

    // SetUp
    char *input = "#0xFA";
    int start = 0;

    int expect_imm_value = 0xFA;

    int actual_imm_value;

    // Exercise
    start = parse_immediate(input, start, &actual_imm_value);

    // Verify
    assert(expect_imm_value == actual_imm_value);
}

/**************** parse raw **********************************/

static void test_parse_raw_when_number_only(){

    // SetUp
    char *input = "0x12345678";
    int start = 0;

    unsigned int expect_imm_value = 0x12345678;

    unsigned int actual_raw_value;

    // Exercise
    start = parse_raw(input, start, &actual_raw_value);

    // Verify
    assert(expect_imm_value == actual_raw_value);
}

static void test_parse_raw_when_number_max(){

    // SetUp
    char *input = "0xFFFFFFFF";
    int start = 0;

    unsigned expect_imm_value = 0xFFFFFFFF;

    unsigned int actual_raw_value;

    // Exercise
    start = parse_raw(input, start, &actual_raw_value);

    // Verify
    assert(expect_imm_value == actual_raw_value);
}


/**************** parse left square bracket ***********************/

static void test_parse_left_sbracket(){

    // SetUp
    char *input = "[";
    int start = 0;

    int expect = 1;

    // Exercise
    int actual = parse_left_sbracket(input, start);

    // Verify
    assert(expect == actual);
}


/**************** parse right square bracket ***********************/

static void test_parse_right_sbracket(){

    // SetUp
    char *input = "]";
    int start = 0;

    int expect = 1;

    // Exercise
    int actual = parse_right_sbracket(input, start);

    // Verify
    assert(expect == actual);
}






/**************** is_XXX  ************************************/

static void test_is_register(){

    // SetUp
    char *input = "r1";
    int start = 0;

    int expect = 1;

    // Exercise
    int actual = is_register(input, start);

    // Verify
    assert(expect == actual);
}

static void test_is_comma(){

    // SetUp
    char *input = ",";
    int start = 0;

    int expect = 1;

    // Exercise
    int actual = is_comma(input, start);

    // Verify
    assert(expect == actual);
}





/**************** asm one ************************************/

static void test_asm_one_when_symbol_is_mov_with_reg(){

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

static void test_asm_one_when_symbol_is_mov_with_immediate(){

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

static void test_asm_one_when_symbol_is_raw_number_only(){

    // SetUp
    char *input = ".raw 0x12345678";
    unsigned int expect = 0x12345678;


    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

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
    emitter.array = g_asm_result;
    emitter.pos = 0;

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
    emitter.array = g_asm_result;
    emitter.pos = 0;

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
    emitter.array = g_asm_result;
    emitter.pos = 0;

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
    emitter.array = g_asm_result;
    emitter.pos = 0;

    // Exercise
    asm_one(input, &emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}

/**************** single atoi hex ************************************/

static void test_single_atoi_hex_with_a() {

    // SetUP
    char *input = "a";
    unsigned int expect = 0xa;
    int actual;

    // Exercise
    actual = single_atoi_hex(input);

    // Verify
    assert(expect == actual);
}

static void test_single_atoi_hex_with_A() {

    // SetUP
    char *input = "A";
    unsigned int expect = 0xa;
    int actual;

    // Exercise
    actual = single_atoi_hex(input);

    // Verify
    assert(expect == actual);
}

static void test_single_atoi_hex_with_1() {

    // SetUP
    char *input = "1";
    unsigned int expect = 0x1;
    int actual;

    // Exercise
    actual = single_atoi_hex(input);

    // Verify
    assert(expect == actual);
}

static void unit_tests() {

    // parse_one
    test_parse_one_when_call_once();
    test_parse_one_when_everything_parse();
    test_parse_one_when_parse_one_colon();
    test_parse_one_with_raw();

    // parse_register
    test_parse_register_when_call_once();
    test_parse_register_when_parse_two_registers();
    test_parse_register_when_parse_fail();

    // parse_immediate
    test_parse_immediate_when_call_once();
    test_parse_immediate_with_leading_space();
    test_parse_immediate_with_hexadecimal();
    test_parse_immediate_with_imm0x64();
    test_parse_immediate_with_imm0xFA();

    // parse_raw
    test_parse_raw_when_number_only();
    test_parse_raw_when_number_max();

    // parse left square bracket
    test_parse_left_sbracket();

    // parse right square bracket
    test_parse_right_sbracket();

    // is_XXX
    test_is_register();
    test_is_comma();


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


    // single_atoi_hex
    test_single_atoi_hex_with_a();
    test_single_atoi_hex_with_A();
    test_single_atoi_hex_with_1();

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