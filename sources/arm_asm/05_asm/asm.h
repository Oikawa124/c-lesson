#include <stdio.h>

#define PARSE_FAIL -1

// 文字列
struct substring {
    char *str;
    int len;
};



/* initialize.c*/
void set_up();

int MOV;
int LDR;
int STR;
int RAW;


/*parse.c*/
int parse_one(char *str, int start, struct substring* out_sub_str);
int parse_register(char *str, int start, int *out_register);
int parse_immediate(char *str, int start, int *out_imm_value);
int parse_raw(char *str, int start, unsigned int *out_raw_value);

int parse_left_sbracket(char *str, int start);
int parse_right_sbracket(char *str, int start);

int is_register(char *str, int start);
int is_comma(char *str, int start);
int is_colon(char *str, int start);

int skip_space(char *str, int start);
int skip_comma(char *str, int start);



/*cl_utils.c*/
void cl_getc_set_src(char *str);
void cl_getc_set_fp(FILE *_fp);

int cl_getline(char **out_buf);
void cl_print_hex_dump(unsigned int word);
int single_atoi_hex(char *str, int *out_num);


/*結果をいれる配列*/
struct Emitter {
    unsigned int *array;
    int pos;
};

void initialize_result_arr(struct Emitter *emitter);
void emit_word(struct Emitter *emitter, unsigned int oneword);


/* binary tree*/
void initialize_mnemonic_root();
int to_mnemonic_symbol(struct substring *substr);

void initialize_label_root();
int to_label_symbol(struct substring *substr);


/*dictionary*/
void dict_put(int key, unsigned int line_num);
int dict_get(int key, unsigned int *out_line_num);
