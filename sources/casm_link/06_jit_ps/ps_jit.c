#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>

#include "parser.h"
#include "test_util.h"
#include "disasm.h"

/*
JIT
*/
int *binary_buf = NULL;

static int initialize_binary_buf(){
    binary_buf = NULL;
}

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void ensure_jit_buf() {
    if(binary_buf == NULL) {
        binary_buf = allocate_executable_buf(1024);
    }
}


// emitter : バイナリをarrayに送るようにする。

void initialize_result_arr(struct Emitter *emitter){
    emitter->array = binary_buf;
    emitter->pos = 0;
}

void emit_word(struct Emitter* emitter, unsigned int oneword){
    emitter->array[emitter->pos] = oneword;
    emitter->pos++;
}

// eval : 文字列をパースしてバイナリをemitする
int eval(struct Emitter *emitter, char *str) {
    struct Substr remain={str, strlen(str)};

    while(!is_end(&remain)) {
        skip_space(&remain);
        if(is_number(remain.ptr)) {

            // スタックに即値をpushするバイナリ
            int oneword_imm = 0xe3a02000 + parse_number(remain.ptr);

            emit_word(emitter, oneword_imm); // mov r2, #5  レジスタをr2で決め打ちする
            emit_word(emitter, 0xe92d0004); // push {r2}

            skip_token(&remain);
            continue;

        }else if(is_register(remain.ptr)) {
            if(remain.ptr[1] == '1') {
                emit_word(emitter, 0xe92d0002); // push {r1}
            } else {
                emit_word(emitter, 0xe92d0001); // push {r0}
            }

            skip_token(&remain);
            continue;

        } else {
            // must be op.
            int val = parse_word(&remain);
            skip_token(&remain);

            emit_word(emitter, 0xe8bd000c);  // pop {r2, r3} 　　r3がarg2, r2がarg1となる

            switch(val) {
                case OP_ADD:
                    emit_word(emitter, 0xe0822003);  // add     r2, r2, r3
                    break;

                case OP_SUB:
                    emit_word(emitter, 0xe0432002);  // sub     r2, r2, r3
                    break;

                case OP_MUL:
                    emit_word(emitter, 0xe0020392);  // mul     r2, r2, r3
                    break;

                case OP_DIV:
                    fprintf(stderr, "Not Implemented\n");
                    //stack_push(arg1/arg2);
                    break;
            }

            emit_word(emitter, 0xe92d0004);  // push    {r2}
            continue;
        }
    }

    /* return stack_pop()　に相当する処理 */

    // スタック (r0)をpopするバイナリ
    emit_word(emitter, 0xe8bd0001);

    // mov, pc, lrするバイナリ
    emit_word(emitter, 0xe1a0f00e);

    return 1;
}


// JITスクリプト
int* jit_script(char *input) {
    ensure_jit_buf();

    //　バイナリを詰める配列を準備
    struct Emitter emitter;
    initialize_result_arr(&emitter);

    // バイナリを配列に詰める
    int res = eval(&emitter, input);

    read_binary_file(&emitter);

    return binary_buf;
}


// My Unit test

static void test_jit_script_input_number_5(){

    // SetUp
    char *input = "5"; // 5だけスタックに残るを想定

    int expect = 5;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0); // 引数は使われない

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();
};


static void test_jit_script_input_add_op(){

    // SetUp
    char *input = "5 4 add";

    int expect = 9;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0); // 引数は使われない

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();

};

static void test_jit_script_input_sub_op(){

    // SetUp
    char *input = "5 4 sub";

    int expect = 1;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0); // 引数は使われない

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();
};

static void test_jit_script_input_mul_op(){

    // SetUp
    char *input = "5 4 mul";

    int expect = 20;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0); // 引数は使われない

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();
};


static void test_jit_script_using_register1(){

    // SetUp
    char *input = "r1"; // r1だけスタックに残るを想定

    int expect = 5;

    int (*funcvar)();
    funcvar = (int(*)(int, int))jit_script(input);

    // Execute
    int actual = funcvar(1, 5); // 1の引数がr0, 5の引数がr1となる。

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();
};

static void test_jit_script_using_register0(){

    // SetUp
    char *input = "r0"; // r0だけスタックに残るを想定

    int expect = 5;

    int (*funcvar)();
    funcvar = (int(*)(int, int))jit_script(input);

    // Execute
    int actual = funcvar(5, 1);

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();

};

static void test_jit_script_input_add_op_long(){

    // SetUp
    char *input = "5 4 add 1 add";

    int expect = 10;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0); // 引数は使われない

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();

};

static void test_jit_script_input_long(){

    // SetUp
    char *input = "3 7 add 1 sub 5 mul";

    int expect = 45;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input);

    // Exercise
    int actual = funcvar(0, 0);

    printf("%d\n", actual);

    // Verify
    assert(expect == actual);

    // TearDown
    initialize_binary_buf();

};



static void unit_tests(){

//    test_jit_script_input_number_5();
    test_jit_script_input_add_op();
//    test_jit_script_input_sub_op();
//    test_jit_script_input_mul_op();
//
//    test_jit_script_using_register1();
//    test_jit_script_using_register0();
//
//    test_jit_script_input_add_op_long();
//    test_jit_script_input_long();

    printf("all test done\n");
}


int main() {

    unit_tests();

//    int res;
//    int (*funcvar)(int, int);
//
//    /*
//     TODO: Make below test pass.
//    */
//    funcvar = (int(*)(int, int))jit_script("3 7 add r1 sub 4 mul");
//
//    res = funcvar(1, 5);
//    assert_int_eq(20, res);
//
//    res = funcvar(1, 4);
//    assert_int_eq(24, res);

    return 0;
}


// todo disasmのaddの実装を修正する。
// レジスタ同士の足し算をできるようにする。
//  mulもか？
// 全体的にdisasmを見直してみたほうがよいかも
