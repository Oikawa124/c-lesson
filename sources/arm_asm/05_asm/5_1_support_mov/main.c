#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#include "asm.h"


#define PRASE_FAIL -1
FILE *fp;


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

    int str_pos = start;
    int ch;

    // 空白スキップ
    while (str[str_pos] == ' ') {
        str_pos++;
    }

    int sub_str_first_pos = str_pos;

    if ((ch = str[str_pos]) == ':'
         || ch == ',') {
        out_sub_str->str = &str[sub_str_first_pos];
        out_sub_str->len = 1;
        str_pos++;

        return str_pos;
    }

    while ((ch = str[str_pos]) !='\0'
            && ch != ' '
            && ch != ':'
            && ch != ',')
    {
        str_pos++;
    }

    out_sub_str->str = &str[sub_str_first_pos];
    out_sub_str->len = str_pos - sub_str_first_pos;

    return str_pos;
}


int parse_register(char *str, int start, int *out_register){
    int ch;
    int pos = start;
    int reg_num = 0;


    // 空白・文字"r"読み飛ばし
    while ((ch = str[pos]) == ' '
            || isalpha(ch))
    {
        pos++;
    }

    // 数字取得
    for (; isdigit(ch); ch = str[pos]) {
        reg_num = reg_num * 10 + (ch - '0');
        pos++;
    }

    if (0 <= reg_num && reg_num <= 15) {
        *out_register = reg_num;
        return pos;
    }

    return PRASE_FAIL;
}

int skip_comma(char *str, int start){

    int ch;
    int pos = start;

    //　スペース読み飛ばし
    while ((ch = str[pos]) == ' ') {
        pos++;
    }

    // コンマ読み飛ばし
    if (ch == ',') {
        pos++;
    }

    if (start < pos) {
        return pos;
    } else {
        return PRASE_FAIL;
    }
}





// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(struct Emitter *emitter){

    // 一行読み込み
    char *buf;
    int buf_len = 0;
    int start = 0;


    buf_len = cl_getline(&buf);


    // 命令切り出し
    struct substring sub_str;


    start = parse_one(buf, start, &sub_str);


    unsigned int oneword = 0;

    if (strncmp(sub_str.str,"mov", 3) == 0) {

        oneword += 0xE1A00000;

        // レジスタ切り出し
        int reg_1st, reg_2nd;

        start = parse_register(buf, start, &reg_1st);

        if (start == PRASE_FAIL) { return start; }

        start = skip_comma(buf, start);

        if (start == PRASE_FAIL) { return start; }

        start = parse_register(buf, start, &reg_2nd);

        if (start == PRASE_FAIL) { return start; }

        oneword += reg_1st << 12 ;

        oneword += reg_2nd;

        emit_word(emitter, oneword);
    }

    return 0;
}




/**************** Unit Tests ****************/


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

static void test_asm_when_symbol_is_mov(){

    // SetUp
    char *input = "mov r1, r2";
    unsigned int expect = 0xE1A01002;

    cl_getc_set_src(input);

    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

    // Exercise
    asm_one(&emitter);
    unsigned int actual = emitter.array[0];

    // Verify
    assert(expect == actual);
}


static void unit_tests() {

    test_parse_one_when_call_once();
    test_parse_one_when_everything_parse();
    test_parse_one_when_parse_one_colon();
    test_parse_register_when_call_once();
    test_parse_register_when_parse_two_registers();
    test_asm_when_symbol_is_mov();
}


int main() {

    unit_tests();

    // 結果を渡す配列を準備

    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;

    fp = fopen("mov_op.ks", "r");

    asm_one(&emitter);

    fclose(fp);

    return 0;
}
