#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>

#include "parser.h"
#include "test_util.h"

extern int eval(int r0, int r1, char *str);

/*
JIT
*/
int *binary_buf = NULL;

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


// ここに実装していく

int* jit_script(char *input) {
    ensure_jit_buf();

    // emit binary in hard code

    binary_buf[0] = 0xe1a00001; // mov r0, r1
    binary_buf[1] = 0xe1a0f00e; // mov r15, r14

    return binary_buf;
}


// My Unit test

static void return_num_of_5(){

    // SetUp
    char *input = "r1"; // r1だけスタックに残るを想定

    int expect = 5;

    int (*funcvar)();
    funcvar = (int(*)(int, int))jit_script(input);

    // Execute
    int actual = funcvar(1, 5); // 1の引数がr0, 5の引数がr1となる。

    // Verify
    assert(expect == actual);

};


static void unit_tests(){

    return_num_of_5();

    printf("all test done\n");
}


int main() {
    //int res;
    //int (*funcvar)(int, int);

    unit_tests();

//    res = eval(1, 5, "3 7 add r1 sub 4 mul");
//    printf("res=%d\n", res);
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
