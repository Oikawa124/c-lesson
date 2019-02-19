#define PARSE_FAIL -1

/*cl_utils.c*/
void cl_getc_set_src(char *str);
void cl_getc_set_fp(FILE *_fp);

int cl_getline(char **out_buf);
void cl_print_hex_dump(unsigned int word);


// 文字列
struct substring {
    char *str;
    int len;
};



/*結果をいれる配列*/
struct Emitter {
    unsigned int *array;
    int pos;
};

void initialize_result_arr(struct Emitter *emitter);
void emit_word(struct Emitter *emitter, unsigned int oneword);


/* my atoi*/
int single_atoi_hex(char *str, int *out_num);



/* binary tree*/
typedef struct _Node {
    char *name;
    int value;
    struct _Node *left;
    struct _Node *right;
} Node;

Node *mnemonic_root = NULL;
Node *label_loot = NULL;

int to_mnemonic_symbol(struct substring *substr);
