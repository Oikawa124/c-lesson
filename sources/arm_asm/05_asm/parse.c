#include <ctype.h>
#include <assert.h>
#include <mem.h>
#include <malloc.h>

#include "asm.h"


int skip_space(char *str, int start){
    int pos = start;

    // スペース読み飛ばし
    while (str[pos] == ' ') { pos++;}

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



// トークン切り出し
int parse_one(char *str, int start, struct substring* out_sub_str){

    int pos = start;
    int ch;

    pos = skip_space(str, pos);

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

    pos = skip_space(str, pos);

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

    pos = skip_space(str, pos);

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
    int res;
    int one_hex;

    while(isxdigit(str[pos])){

        if (hex_num != 0) {
            hex_num = hex_num << 4;
        }

        res = single_atoi_hex(&str[pos], &one_hex);

        if (res == PARSE_FAIL) {return PARSE_FAIL; }

        hex_num += one_hex;

        pos++;
    }

    if (is_negative_num) {
        hex_num = -1*hex_num;
    }

    *out_imm_value = hex_num;

    return pos;
}


int parse_raw_value(char *str, int start, unsigned int *out_raw_value){
    int pos = start;

    pos = skip_space(str, pos);

    // "0"読み飛ばし
    if (str[pos] != '0') { return PARSE_FAIL;}
    pos++;

    // "x"読み飛ばし
    if (str[pos] != 'x') { return PARSE_FAIL;}
    pos++;


    // 数字部分を取得
    unsigned int hex_num = 0;
    int res;
    int one_hex;

    while(isxdigit(str[pos])){

        if (hex_num != 0) {
            hex_num = hex_num << 4;
        }

        res = single_atoi_hex(&str[pos], &one_hex);

        if (res == PARSE_FAIL) {return PARSE_FAIL; }

        hex_num += one_hex;

        pos++;
    }

    *out_raw_value = hex_num;
    return pos;
}


/*********************** parse string***************************************************/

static char tmp_buf[1024];
static int tmp_cnt = 0;

// 文字列ステートマシン
static int state;

typedef enum {
    ST_INIT,
    ST_STRING,
    ST_ESCAPE,
    ST_END,
} State;




//todo tmp_bufに詰めていくようにする
int parse_string(struct substring *input, int start, char **out_str_value) {

    int pos = start;
    char tmp_buf[1024];
    int tmp_cnt = 0;

    pos = skip_space(input->str, pos);

    state = ST_INIT;

    while (state != ST_END){
        int ch = input->str[pos++];

        switch (state) {
            case ST_INIT:
                if (ch != '"') {
                    return PARSE_FAIL;
                }
                state = ST_STRING;
                continue;

            case ST_STRING:
                if (ch == '\\') {
                    state = ST_ESCAPE;
                    continue;
                } else if (ch == '"') {
                    state = ST_END;
                    continue;
                } else {
                    tmp_buf[tmp_cnt++] = (char)ch;
                    continue;
                }
            case ST_ESCAPE:
                tmp_buf[tmp_cnt++] = (char)ch;
                state = ST_STRING;
                continue;
            case ST_END:
                break;

            default:
                break;
        }
    }

    tmp_buf[tmp_cnt] = '\0';

    char *res = (char *) malloc(sizeof(char) * (tmp_cnt+1));
    memcpy(res, tmp_buf, tmp_cnt+1);

    *out_str_value = res;

    return pos;
}


/***********************************************************************************/

int parse_left_sbracket(char *str, int start){
    int pos = start;

    pos = skip_space(str, pos);

    // 左角括弧読み飛ばし
    if (str[pos] != '[') { return PARSE_FAIL; }
    pos++;

    return pos;
}

int parse_right_sbracket(char *str, int start){
    int pos = start;

    pos = skip_space(str, pos);

    // 右角括弧読み飛ばし
    if (str[pos] != ']') { return PARSE_FAIL; }
    pos++;

    return pos;
}




int is_register(char *str, int start) {
    int pos = start;

    pos = skip_space(str, pos);

    if (str[pos] == 'r') {
        return 1;
    }
    else {
        return 0;
    }
}

int is_comma(char *str, int start) {
    int pos = start;

    pos = skip_space(str, pos);

    if (str[pos] == ',') {
        return 1;
    }
    else {
        return 0;
    }
}


int is_colon(char *str, int start) {
    int pos = start;

    pos = skip_space(str, pos);

    if (str[pos] == ':') {
        return 1;
    }
    else {
        return 0;
    }
}




/**************** Unit Tests ************************************/

int strneq(char *s1, char *s2, int len) { return 0 == strncmp(s1, s2, len); }
//static int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }

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
    start = parse_raw_value(input, start, &actual_raw_value);

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
    start = parse_raw_value(input, start, &actual_raw_value);

    // Verify
    assert(expect_imm_value == actual_raw_value);
}



/**************** parse string *************************************/



static void test_parse_string_when_call_once() {

    // SetUp
    struct substring input = {.str="\"hi\"", .len=2};
    int start = 0;

    char *expect = "hi";
    char *actual;

    // Exercise
    start = parse_string(&input, start, &actual);

    // Verify
    assert(strneq(expect, actual, 2));
}

static void test_parse_string_when_call_once_2() {

    // SetUp
    struct substring input = {.str="\"hello\\\n\"", .len=9};
    int start = 0;

    char *expect = "hello\n";
    char *actual;

    // Exercise
    start = parse_string(&input, start, &actual);

    // Verify
    assert(strneq(expect, actual, 6));
}

static void test_parse_string_when_new_line() {

    // SetUp
    struct substring input = {.str="\"\\\n\"", .len=3};
    int start = 0;

    char *expect = "\n";
    char *actual;

    // Exercise
    start = parse_string(&input, start, &actual);

    // Verify
    assert(strneq(expect, actual, 1));
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

    // parse_string
    test_parse_string_when_call_once();
    test_parse_string_when_call_once_2();
    test_parse_string_when_new_line();

    // parse_raw_value
    test_parse_raw_when_number_only();
    test_parse_raw_when_number_max();

    // parse left square bracket
    test_parse_left_sbracket();

    // parse right square bracket
    test_parse_right_sbracket();

    // is_XXX
    test_is_register();
    test_is_comma();
}

int main() {
    unit_tests();

    return 0;
}
