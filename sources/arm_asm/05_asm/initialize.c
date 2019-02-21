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
    RAW = str_to_mnemonic_symbol(".raw", 4);
}
