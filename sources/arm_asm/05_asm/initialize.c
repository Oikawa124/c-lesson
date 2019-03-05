#include <assert.h>
#include "asm.h"

static int str_to_mnemonic_symbol(char *str, int len) {
    struct substring sub;
    sub.str = str;
    sub.len = len;

    int value = to_mnemonic_symbol(&sub);

    return value;
}

void set_up(){
    initialize_mnemonic_root();

    MOV = str_to_mnemonic_symbol("mov", 3);
    STR = str_to_mnemonic_symbol("str", 3);
    LDR = str_to_mnemonic_symbol("ldr", 3);
    B   = str_to_mnemonic_symbol("b", 1);

    RAW = str_to_mnemonic_symbol(".raw", 4);

}

void initialize_when_test(){
    free_linked_list();
    initialize_label_root();
    initialize_dict();
}


/*unit tests*/

static void test_str_to_mnemonic_symbol_when_call_once(){
    // SetUP
    char *input = "mov";
    int len = 3;

    int expect_value = 1;

    // Exercise
    int actual_value = str_to_mnemonic_symbol(input, len);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_mnemonic_root();
}

static void test_str_to_mnemonic_symbol_when_call_three_times(){
    // SetUP
    char *input1 = "mov";
    int len1 = 3;

    char *input2 = "str";
    int len2 = 3;

    char *input3 = "mov";
    int len3 = 3;

    int expect_value1 = 1;
    int expect_value2 = 2;
    int expect_value3 = 1;


    // Exercise
    int actual_value1 = str_to_mnemonic_symbol(input1, len1);
    int actual_value2 = str_to_mnemonic_symbol(input2, len2);
    int actual_value3 = str_to_mnemonic_symbol(input3, len3);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);
    assert(expect_value3 == actual_value3);

    // TearDown
    initialize_mnemonic_root();
}


static void unit_tests(){
    test_str_to_mnemonic_symbol_when_call_once();
    test_str_to_mnemonic_symbol_when_call_three_times();
}


//int main(){
//    unit_tests();
//    return 0;
//}