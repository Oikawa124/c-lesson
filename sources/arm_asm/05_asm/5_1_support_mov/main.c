#include <stdio.h>
#include <mem.h>
#include <ctype.h>

#define BUF_SIZE 100


enum TokenType {
    mov,
};

struct substring {
    char *str;
    int len;
};


struct Token {
    enum TokenType t_type;
    struct substring sub;
};



// 1行読み込んだ文字列のバッファ
static char buf[BUF_SIZE];

// 一行読み込み
int cl_getline(char **out_buf, FILE *fp) {
    int len = 0;
    int ch;

    if ((ch = getc(fp)) == EOF) {return EOF;}

    do {
        buf[len] = ch;
        len++;
    } while ((ch = getc(fp)) != '\n');

    buf[len] = '\0';

    *out_buf = buf;

    return len;
}


// 先頭のシンボルを切り出す
int parse_one(char *str, struct Token* out_token){

    int len = 0;
    int ch;
    char word_buf[BUF_SIZE];


    do {
        ch = str[len];
        word_buf[len] = (char)ch;

        len++;

    } while (!isspace(ch) && isalnum(ch));

    word_buf[len] = '\0';


    if (strcmp(word_buf, "mov")) {
        out_token->t_type = mov;
    } else {
        printf("not complement");
    }

    out_token->sub.str = str;
    out_token->sub.len = len-1;

    return len;
}


int parse_register(int read_len, char *str, int *out_register){
    int ch;
    int len = read_len;
    int reg_mun = 0;

    // 空白・文字"r"読み飛ばし
    do {
        ch = str[len];
        len++;
    } while (isspace(ch) || isalpha(ch));

    for (; isdigit(ch); ch = str[len]) {
        reg_mun = reg_mun * 10 + (ch - '0');
        len++;
    }

    *out_register = reg_mun;

    return len;
}



// 先頭のトークンを読み出して，結果によって分岐する
void asm_one(FILE *fp){


    // 一行読み込み
    char *out_buf;
    int len = cl_getline(&out_buf, fp);



    // 命令切り出し
    struct Token symbol;

    int read_len = parse_one(out_buf, &symbol);


    // レジスタ切り出し
    int reg_1st;

    read_len = parse_register(read_len, out_buf, &reg_1st);



    fclose(fp);

}


int main() {

    FILE *fp = fopen("mov_op.txt", "r");

    asm_one(fp);

    fclose(fp);

    return 0;
}
