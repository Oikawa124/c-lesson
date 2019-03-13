#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "asm.h"

#define BUF_SIZE 100
static FILE *fp;


/* 文字列読み込み関係*/
static char *input = NULL;
static int pos = 0;

void cl_getc_set_src(char *str) {
    input = str;
    pos = 0;
}

void cl_getc_set_fp(FILE *_fp) {
    fp = _fp;
}

static int cl_getc() {
    if (fp) {
        return fgetc(fp);
    } else {
        if (strlen(input) == pos) {
            return EOF;
        }
        return input[pos++];
    }
}

// g_は"global"
static char g_buf[BUF_SIZE];

// 一行読み込み
int cl_getline(char **out_buf) {
    int len = 0;
    int ch;

    if ((ch = cl_getc()) == EOF) {return EOF;}

    do {
        g_buf[len] = (char)ch;
        len++;

    } while ((ch = cl_getc()) != '\n' && ch != EOF);

    g_buf[len] = '\0';

    *out_buf = g_buf;

    return len;
}


/* 16進数ダンプ */
void cl_print_hex_dump(unsigned int word){
    for (int i = 0; i < 4; ++i) {
        int two_digit_word = word & 0xFF;

        if (i != 3) {
            printf("%02x ", two_digit_word);
        } else {
            printf("%02x", two_digit_word);
        }
        word = word >> 2*4;
    }
}


/* アセンブラ結果の出力関係*/
static unsigned int g_asm_result[1000];

void initialize_result_arr(struct Emitter *emitter){
    emitter->array = g_asm_result;
    emitter->pos = 0;
}

void emit_word(struct Emitter* emitter, unsigned int oneword){
    emitter->array[emitter->pos] = oneword;
    emitter->pos++;
}

unsigned int get_last_memory_address(struct Emitter *emitter) {

    // emit_wordを呼ぶとposは次のバイナリを入れる場所になるため、-1する。
    unsigned int address = 0x00010000 + (unsigned int)(emitter->pos-1) * 4;

    return address;
}



/*自作した標準関数もどき*/
int single_atoi_hex(char *str, int *out_num) {

    if (('0' <= *str) && (*str <= '9')) {
        *out_num = *str - 48;
        return 1;
    } else if (('a' <= *str) && (*str <= 'f')) {
        *out_num = *str - 87;
        return 1;
    } else if (('A' <= *str) && (*str <= 'F')) {
        *out_num = *str - 55;
        return 1;
    } else {
        return PARSE_FAIL;
    }
}





/**************** single atoi hex ************************************/

static void test_single_atoi_hex_with_a() {

    // SetUP
    char *input = "a";
    unsigned int expect = 0xa;

    int actual;
    int res;

    // Exercise
    res = single_atoi_hex(input, &actual);

    // Verify
    assert(expect == actual);
}

static void test_single_atoi_hex_with_A() {

    // SetUP
    char *input = "A";
    unsigned int expect = 0xa;

    int actual;
    int res;

    // Exercise
    res = single_atoi_hex(input, &actual);

    // Verify
    assert(expect == actual);
}

static void test_single_atoi_hex_with_1() {

    // SetUP
    char *input = "1";
    unsigned int expect = 0x1;

    int actual;
    int res;

    // Exercise
    res = single_atoi_hex(input, &actual);

    // Verify
    assert(expect == actual);
}


static void unit_tests(){

    // single_atoi_hex
    test_single_atoi_hex_with_a();
    test_single_atoi_hex_with_A();
    test_single_atoi_hex_with_1();
}

//
//int main() {
//    unit_tests();
//
//    return 0;
//}