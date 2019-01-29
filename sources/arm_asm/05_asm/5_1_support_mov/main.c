#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <assert.h>

#define BUF_SIZE 100
#define PRASE_FAIL -1



struct substring {
    char *str;
    int len;
};


// 1行読み込んだ文字列のバッファ
static char buf[BUF_SIZE];

// 一行読み込み
int cl_getline(char **out_buf, FILE *fp) {
    int len = 0;
    int ch;

    if ((ch = getc(fp)) == EOF) {return EOF;}

    do {
        buf[len] = (char)ch;
        len++;

    } while ((ch = getc(fp)) != '\n' && ch != EOF);

    buf[len] = '\0';

    *out_buf = buf;

    return len;
}


// 先頭のシンボルを切り出す (ラベルや.rawは後で実装していく)
int parse_one(int out_buf_pos, char *str, struct substring* out_sub_str){

    int str_pos = out_buf_pos;
    int ch;

    if ((ch = str[str_pos]) == ':'
         || ch == ',') {
        out_sub_str->str = &str[out_buf_pos];
        out_sub_str->len = 1;
        str_pos++;

        return str_pos;
    }

    // 空白スキップ
    while (str[str_pos] == ' ') {
        str_pos++;
    }


    int sub_str_first_pos = str_pos;

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


int parse_register(int str_pos, char *str, int *out_register){
    int ch;
    int pos = str_pos;
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

int skip_comma(int str_pos, char *str){

    // コンマ読み飛ばし
    int ch;
    int pos = str_pos;

    do {
        ch = str[pos];
        pos++;
    } while (ch == ',');

    if (str_pos < pos) {
        return pos;
    } else {
        return PRASE_FAIL;
    }
}





// 先頭のトークンを読み出して，結果によって分岐する
int asm_one(FILE *fp){

    // 一行読み込み
    char *out_buf;
    int out_buf_len = 0;
    int out_buf_pos = 0;


    out_buf_len = cl_getline(&out_buf, fp);


    // 命令切り出し
    struct substring sub_str;


    out_buf_pos = parse_one(out_buf_pos, out_buf, &sub_str);


//    // レジスタ切り出し
//    int reg_1st, reg_2nd;
//
//    out_buf_pos = parse_register(out_buf_pos, out_buf, &reg_1st);
//
//    if (out_buf_pos == PRASE_FAIL) { return out_buf_pos;}
//
//    out_buf_pos = skip_comma(out_buf_pos, out_buf);
//
//    if (out_buf_pos == PRASE_FAIL) { return out_buf_pos;}
//
//    out_buf_pos = parse_register(out_buf_pos, out_buf, &reg_2nd);
//
//    if (out_buf_pos == PRASE_FAIL) { return out_buf_pos;}


    return 0;
}




/**************** Unit Tests ****************/


int strneq(char *s1, char *s2, int len) { return 0 == strncmp(s1, s2, len); }

void assert_substring_eq(char *expect, struct substring* actual){
    assert(strneq(expect, actual->str, actual->len));
}

static void test_parse_one() {

    char *input = "loop: mov r1, r2";
    int input_pos = 0;

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


    input_pos = parse_one(input_pos,input, &actual1);
    input_pos = parse_one(input_pos,input, &actual2);
    input_pos = parse_one(input_pos,input, &actual3);
    input_pos = parse_one(input_pos,input, &actual4);
    input_pos = parse_one(input_pos,input, &actual5);
    input_pos = parse_one(input_pos,input, &actual6);

    assert_substring_eq(expect1, &actual1);
    assert_substring_eq(expect2, &actual2);
    assert_substring_eq(expect3, &actual3);
    assert_substring_eq(expect4, &actual4);
    assert_substring_eq(expect5, &actual5);
    assert_substring_eq(expect6, &actual6);
}


static void test_parse_register() {
    char *input = " r1, r2";
    int input_pos = 0;

    int expect_reg_1 = 1;
    int expect_reg_2 = 2;

    int actual_reg_1, actual_reg_2;


    input_pos = parse_register(input_pos, input, &actual_reg_1);

    input_pos = skip_comma(input_pos, input);

    input_pos = parse_register(input_pos, input, &actual_reg_2);

    assert(expect_reg_1 == actual_reg_1);
    assert(expect_reg_2 == actual_reg_2);
}


static void unit_tests() {

    test_parse_one();
    test_parse_register();
}


int main() {

    unit_tests();

//    FILE *fp = fopen("mov_op.ks", "r");
//
//    asm_one(fp);
//
//    fclose(fp);

    return 0;
}
