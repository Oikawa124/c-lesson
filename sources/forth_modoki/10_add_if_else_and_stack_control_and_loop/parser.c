#include "clesson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_SIZE 256
#define CONTINUE 1

static struct ElementArray *exec_array = NULL;
static int operation_pos = 0;


int _isdigit(int n) { return '0' <= n && n <= '9';}

int _isLetter(int ch){ return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';}

static int parse_one(int prev_ch, struct Token *out_token) {

    int ch;

    if (prev_ch == EOF){
        ch = cl_getc();
    }else{
        ch = prev_ch;
    }

    if (_isdigit(ch)) {
        int num = 0;
        for (; _isdigit(ch); ch = cl_getc()) {
            num = num * 10 + (ch - '0');
        }

        out_token->ltype = NUMBER;
        out_token->u.number = num;
        return ch;

    } else if (_isLetter(ch)) {
        char *str;
        int size = NAME_SIZE;

        str = malloc(sizeof(char) * size);

        int i = 0;
        for (; _isLetter(ch) || _isdigit(ch); ch = cl_getc()) {
            str[i] = (char) ch;
            i++;
        }
        str[i] = '\0';

        out_token->u.name = str;
        out_token->ltype = EXECUTABLE_NAME;
        return ch;

    } else if (ch == '/') {
        char *str;
        int size = NAME_SIZE;

        str = malloc(sizeof(char) * size);

        int i = 0;
        ch = cl_getc();
        for (; _isLetter(ch) || _isdigit(ch); ch = cl_getc()) {
            str[i] = (char) ch;
            i++;
        }
        str[i] = '\0';

        out_token->u.name = str;
        out_token->ltype = LITERAL_NAME;
        return ch;

    } else if (ch == ' ') {
        while (ch == ' ') { ch = cl_getc(); }

        out_token->ltype = SPACE;
        out_token->u.onechar = ' ';
        return ch;

    } else if (ch == '{') {
        out_token->ltype = OPEN_CURLY;
        out_token->u.onechar = (char)ch;
        ch = cl_getc();
        return ch;

    } else if (ch == '}') {
        out_token->ltype = CLOSE_CURLY;
        out_token->u.onechar = (char) ch;
        ch = cl_getc();
        return ch;

    }else if (ch == '-'){
        ch = cl_getc();
        int num = 0;
        for (; _isdigit(ch); ch = cl_getc()) {
            num = num * 10 + (ch - '0');
        }

        out_token->ltype = NUMBER;
        out_token->u.number = -num;
        return ch;
    } else if (ch == EOF) {
        out_token->ltype = END_OF_FILE;
        return EOF;
    }

    out_token->ltype = UNKNOWN;
    return EOF;
}


void set_cont(struct Continuation *cont) {
    exec_array = cont->exec_array;
    operation_pos = cont->pc;
}


int get_next_token(int prev_ch, struct Token *out_token, int *out_op_pos){
    if (exec_array == NULL) {

        if (prev_ch == CONTINUE){
            prev_ch = EOF;
        }
        return parse_one(prev_ch, out_token);
    }

    if (exec_array->len == 0){
        fprintf(stderr, "空の実行可能配列です\n");
    }

    struct Element *cur = &exec_array->elements[operation_pos];
    operation_pos++;
    *out_op_pos = operation_pos;

    switch (cur->etype){
        case ELEMENT_NUMBER:
            out_token->ltype = NUMBER;
            out_token->u.number = cur->u.number;
            break;
        case ELEMENT_LITERAL_NAME:
            out_token->ltype = LITERAL_NAME;
            out_token->u.name = cur->u.name;
            break;
        case ELEMENT_EXECUTABLE_NAME:
            out_token->ltype = EXECUTABLE_NAME;
            out_token->u.name = cur->u.name;
            break;
    }

    // 実行可能配列が実行し終わった。
    if (exec_array->len == operation_pos){
        exec_array = NULL;
        operation_pos = 0;
        co_pop();
        return EOF;
    }
    return CONTINUE;
}


void parser_print_all() {
    int ch = EOF;
    struct Token token = {
            UNKNOWN,
            {0}
    };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                    printf("num: %d\n", token.u.number);
                    break;
                case SPACE:
                    printf("space!\n");
                    break;
                case OPEN_CURLY:
                    printf("Open curly brace '%c'\n", token.u.onechar);
                    break;
                case CLOSE_CURLY:
                    printf("Close curly brace '%c'\n", token.u.onechar);
                    break;
                case EXECUTABLE_NAME:
                    printf("EXECUTABLE_NAME: %s\n", token.u.name);
                    break;
                case LITERAL_NAME:
                    printf("ELEMENT_LITERAL_NAME: %s\n", token.u.name);
                    break;
                case NEW_LINE:
                    printf("NEW_LINE\n");
                    break;

                default:
                    printf("Unknown type %d\n", token.ltype);
                    break;
            }
        }
    }while(ch != EOF);
}


static void test_parse_one_number() {
    char *input = "123";
    int expect = 123;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == NUMBER);
    assert(expect == token.u.number);
}

static void test_parse_one_empty_should_return_END_OF_FILE() {
    char *input = "";
    int expect = END_OF_FILE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect);
}

int streq(char *s1, char *s2){ return 0==strcmp(s1, s2);}

static void verify_execname_pares_one(char *expect_name, char *input){
    char *_input = input;
    int expect_type = EXECUTABLE_NAME;
    char *_expect_name = expect_name;

    struct Token token = {UNKNOWN, {0}};

    cl_getc_set_src(_input);
    parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert(streq(_expect_name, token.u.name));
}

static void test_parse_one_executable_name(){
    verify_execname_pares_one("abc", "abc");
    verify_execname_pares_one("abc123", "abc123");
    verify_execname_pares_one("abc_def", "abc_def");
    verify_execname_pares_one("abc", "abc def");
}

static void test_parse_one_literal_name(){
    char *input = "/add";
    int expect_type = LITERAL_NAME;
    char *expect_name = "add";

    struct Token token = {UNKNOWN, {0}};

    cl_getc_set_src(input);
    parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert(streq(expect_name, token.u.name));
}

static void test_parse_one_open_curly(){
    char *input = "{";
    int expect_type = OPEN_CURLY;
    char expect_onechar = '{';

    struct Token token = {UNKNOWN, {0}};

    cl_getc_set_src(input);
    parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert(expect_onechar == token.u.onechar);
}

static void test_parse_one_close_curly(){
    char *input = "}";
    int expect_type = CLOSE_CURLY;
    char expect_onechar = '}';

    struct Token token = {UNKNOWN, {0}};

    cl_getc_set_src(input);
    parse_one(EOF, &token);

    assert(token.ltype == expect_type);
    assert(expect_onechar == token.u.onechar);
}

void test_parse_one_minus_number(){
    char *input = "-123";
    int expect = -123;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == NUMBER);
    assert(expect == token.u.number);
}

void test_parse_one_newline(){
    char *input = "123\n456";
    int expect = 456;

    struct Token token1 = {UNKNOWN, {0}};
    struct Token token2 = {UNKNOWN, {0}};
    struct Token token3 = {UNKNOWN, {0}};

    int ch;
    cl_getc_set_src(input);

    ch = parse_one(EOF, &token1);
    ch = parse_one(ch, &token2);
    parse_one(ch, &token3);

    assert(token2.ltype == SPACE);
    assert(token3.ltype == NUMBER);
    assert(token3.u.number == expect);
}

void test_parse_comment(){
    char *input = "123%This is comment\n456";
    int expect = 456;

    struct Token token1 = {UNKNOWN, {0}};
    struct Token token2 = {UNKNOWN, {0}};
    struct Token token3 = {UNKNOWN, {0}};

    int ch;
    cl_getc_set_src(input);

    ch = parse_one(EOF, &token1);
    ch = parse_one(ch, &token2);
    parse_one(ch, &token3);

    assert(token2.ltype == SPACE);
    assert(token3.ltype == NUMBER);
    assert(token3.u.number == expect);
}

static void unit_tests() {
//    test_parse_one_executable_name();
//    test_parse_one_literal_name();
//    test_parse_one_empty_should_return_END_OF_FILE();
//    test_parse_one_number();
//    test_parse_one_open_curly();
//    test_parse_one_close_curly();
//    test_parse_one_minus_number();
    test_parse_one_newline();
    test_parse_comment();
}


//int main() {
//    unit_tests();
//
////    cl_getc_set_src("123 45 add /some { 2 3 add } def");
////    parser_print_all();
//    return 1;
//}
