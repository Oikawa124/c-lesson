#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum LexicalType {
    NUMBER,
    SPACE
};

int _isdigit(int n) { return '0' <= n && n <= '9';}

int parse_one(int pre_val, int *val_type, int *val)
{

    int ch;

    if (pre_val == EOF){
        ch = cl_getc();
    } else{
        ch = pre_val;
    }

    if (ch == EOF) return EOF;


    if (_isdigit(ch)){
        int num=0;
        for (num; _isdigit(ch); ch=cl_getc()){
            num = num * 10 + (ch - '0');
        }

        *val_type = NUMBER;
        *val = num;
        return ch;

    } else if (ch == ' ') {
        while (ch == ' ') {ch = cl_getc();}

        *val_type = SPACE;
        *val = ' ';
        return ch;
    }
}

void test_parse_one_123()
{
    char *input = "123";
    int expect = 123;

    cl_getc_set_src(input);

    int pre_val=EOF;
    int actual, val_type;

    parse_one(pre_val, &val_type, &actual);

    assert(expect == actual);

}



int main() {
    int answer1 = 0;
    int answer2 = 0;


    // write something here.

    test_parse_one_123();

    // 入力をもとに戻す
    cl_getc_set_src("123 456")


    int pre_val=EOF;
    int val, val_type;

    pre_val = parse_one(pre_val, &val_type, &val);

    answer1 = val;

    pre_val = parse_one(pre_val, &val_type, &val);
    parse_one(pre_val, &val_type, &val);

    answer2 = val;


//    // sample for cl_getc() usage.
//    int c;
//
//    while((c = cl_getc()) != EOF) {
//        printf("%c\n",c );
//    }

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 1;

}