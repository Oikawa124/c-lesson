#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#define BUF_SIZE 100
#define PRASE_FAIL -1


// 入力関係

static char *input = NULL;
static int pos = 0;

static FILE *fp;

void cl_getc_set_src(char *str) {
    input = str;
    pos = 0;
}

int cl_getc() {
    if (fp) {
        return fgetc(fp);
    } else {
        if (strlen(input) == pos)
            return EOF;
        return input[pos++];
    }
}



// 配列関係
static unsigned int g_asm_result[1000];

unsigned int cl_get_asm_result(int num) {
    return g_asm_result[num];
}

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


// 1行読み込んだ文字列のバッファ
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


// 先頭のシンボルを切り出す (ラベルや.rawは後で実装していく)
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
    int reg_mun = 0;

    // 空白・文字"r"読み飛ばし
    do {
        ch = str[pos];
        pos++;
    } while (isspace(ch) || isalpha(ch));

    // 数字取得
    for (; isdigit(ch); ch = str[pos]) {
        reg_mun = reg_mun * 10 + (ch - '0');
        pos++;
    }

    if (0 <= reg_mun && reg_mun <= 15) {
        *out_register = reg_mun;
        return pos;
    }

    return PRASE_FAIL;
}

int skip_comma(char *str, int start){

    // コンマ読み飛ばし
    int ch;
    int pos = start;

    do {
        ch = str[pos];
        pos++;
    } while (ch == ',');

    if (start < pos) {
        return pos;
    } else {
        return PRASE_FAIL;
    }
}





// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(){

    // 一行読み込み
    char *buf;
    int buf_len = 0;
    int start = 0;


    buf_len = cl_getline(&buf);


    // 結果を渡す配列を準備

    struct Emitter emitter;
    emitter.array = g_asm_result;
    emitter.pos = 0;



    // 命令切り出し
    struct substring sub_str;


    start = parse_one(buf, start, &sub_str);

    // int expect = 0xE1A01002;

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

        emit_word(&emitter, oneword);
    }



    return 0;
}




/**************** Unit Tests ****************/


int strneq(char *s1, char *s2, int len) { return 0 == strncmp(s1, s2, len); }

void assert_substring_eq(char *expect, struct substring* actual){
    assert(strneq(expect, actual->str, actual->len));
}

static void test_parse_one() {

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


    start = parse_one(input,start, &actual1);
    start = parse_one(input,start, &actual2);
    start = parse_one(input,start, &actual3);
    start = parse_one(input,start, &actual4);
    start = parse_one(input,start, &actual5);
    start = parse_one(input,start, &actual6);

    assert_substring_eq(expect1, &actual1);
    assert_substring_eq(expect2, &actual2);
    assert_substring_eq(expect3, &actual3);
    assert_substring_eq(expect4, &actual4);
    assert_substring_eq(expect5, &actual5);
    assert_substring_eq(expect6, &actual6);
}

static void test_parse_one_colon(){
    char *input = " :";
    int start = 0;

    char *expect = ":";

    struct substring actual;

    start = parse_one(input, start, &actual);

    assert_substring_eq(expect, &actual);
}


static void test_parse_register() {
    char *input = " r1, r2";
    int start = 0;

    int expect_reg_1 = 1;
    int expect_reg_2 = 2;

    int actual_reg_1, actual_reg_2;


    start = parse_register(input,start, &actual_reg_1);

    start = skip_comma(input, start);

    start = parse_register(input, start, &actual_reg_2);

    assert(expect_reg_1 == actual_reg_1);
    assert(expect_reg_2 == actual_reg_2);
}

static void test_asm_one(){
    char *input = "mov r1, r2";
    unsigned int expect = 0xE1A01002;

    cl_getc_set_src(input);

    asm_one();

    unsigned int actual = cl_get_asm_result(0);

    assert(expect == actual);
}


static void unit_tests() {

    test_parse_one();
    test_parse_one_colon();
    test_parse_register();
    test_asm_one();
}


int main() {

    unit_tests();

//    fp = fopen("mov_op.ks", "r");
//
//    asm_one();
//
//    fclose(fp);

    return 0;
}
