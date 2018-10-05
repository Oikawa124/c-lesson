#include <stdio.h>
#include <string.h>
#define STACK_SIZE 1024


enum LexicalType {
    NUMBER,
    SPACE,
    EXECUTABLE_NAME,
    LITERAL_NAME,
    OPEN_CURLY,
    CLOSE_CURLY,
    END_OF_FILE,
    UNKNOWN
};


struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
    } u;
};

enum ElementType {
    ELEMENT_NUMBER,
    ELEMENT_LITERAL_NAME,
    NO_ELEMENT
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char *name;
    } u;
};

struct Element stack[STACK_SIZE];
int stack_pos;

int cl_getc();

void cl_getc_set_src(char* str);


// パーサー関係の関数
void parser_print_all();

int parse_one(int prev_ch, struct Token *out_token);

int streq(char *s1, char *s2);


// スタック関係の関数
void stack_push(struct Element *token);

void stack_pop(struct Element *out_element);

void stack_add();

void stack_print_all();

void stack_init();

void stack_clear();