#include <stdio.h>
#include <string.h>

#define STACK_SIZE 1024

enum LexicalType {
    NUMBER,
    SPACE,
    EXECUTABLE_NAME,
    LITERAL_NAME,
    EXECUTABLE_ARRAY,
    OPEN_CURLY,
    CLOSE_CURLY,
    END_OF_FILE,
    UNKNOWN,
};


struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
        struct ElementArray *byte_codes;
    } u;
};

enum ElementType {
    ELEMENT_NUMBER,
    ELEMENT_LITERAL_NAME,
    ELEMENT_EXECUTABLE_NAME,
    ELEMENT_C_FUNC,
    ELEMENT_EXECUTABLE_ARRAY,
    NO_ELEMENT,
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char *name;
        void (*cfunc)();
        struct ElementArray *byte_codes;
    } u;
};

struct ElementArray {
    int len;
    struct Element elements[0];
};

struct Continuation {
    struct ElementArray *exec_array;
    int pc;
};


int cl_getc();

void cl_getc_set_src(char* str);

void cl_getc_set_fp(FILE *_fp);


// パーサー関係の関数
void parser_print_all();

int get_next_token(int prev_ch, struct Token *out_token, int *out_op_pos);

void set_cont(struct Continuation *cont);

int streq(char *s1, char *s2);

void init_exec_array();


// スタック関係の関数
void stack_push(struct Element *token);

void stack_pop(struct Element *out_element);

void stack_value_copy(struct Element *out_element, int index);

void stack_print_all();

void stack_init();

void stack_clear();

// eval関係
void eval();

void eval_exec_array();

void init();

// 辞書関係の関数
void dict_put(char *key, struct Element *elem);

int dict_get(char *key, struct Element *out_elem);

void dict_print_all();

void dict_print_one_value(char *key);

// co_stack関係
void co_push(struct Continuation *cont);

void co_pop();

struct Continuation *co_peek();

int co_stack_is_empty();

void set_current_op_pos(int out_op_pos);

void co_push_elem_arr(struct Element *elem_arr);


// プリミティブ関連
void register_primitives();

struct Element *get_exec_array_pointer();

void init_exec_array_pointer();
