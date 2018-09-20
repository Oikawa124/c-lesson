#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum LexicalType {
    NUMBER,
    SPACE
};


// 数字かどうかの判定
int _isdigit(char c)
{

    if (c >= '0' && c <= '9'){
        return 1;
    }else{
        return 0;
    }
}

// 複数桁の文字を数字の変換
int _atoi(char *s)
{
    int i, n;
    i = 0;

    for(n=0; _isdigit(s[i]); i++){
        if(_isdigit(s[i])) {
            n = 10 * n + (s[i] - '0');
        }
    }
    return n;
}


char space_check;
char letter;

int parse_one(int *out_val)
{
    int i = 0;
    char word[100];

    if (space_check == ' '){
        space_check = -5;
        return SPACE;
    }


    if (space_check != -5){
        letter = cl_getc();
    }


    while (letter != ' ' && letter != EOF ){
        word[i] = (char)letter;
        i++;
        letter = cl_getc();
    }


    while(letter == ' '){
        space_check = (char)letter;
        letter = cl_getc();
    }

    if (word[0] != ' ' && word[0] !=EOF){
        *out_val = _atoi(word);
        return NUMBER;
    }
}

int main() {
    int answer1 = 0;
    int answer2 = 0;


    // write something here.
    int out_val;

    parse_one(&out_val);

    answer1 = out_val;

    parse_one(&out_val);
    parse_one(&out_val);

    answer2 = out_val;


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