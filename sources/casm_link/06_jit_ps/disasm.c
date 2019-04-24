#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "disasm.h"

struct register_array{
    int list[16];
    int len;
};

void print_data_transfer(char *mnemonic, int word);
void print_branch_and_branch_with_link(char *mnemonic, int word);
void print_block_data_transfer(char *mnemonic, int word);
void print_data_process(char *mnemonic, int word);


int print_asm(int word) {

    if (0xE3A00000 == (word & 0xFFF00000) ||
        0xE1A00000 == (word & 0xFFF00000)){

        // movまたはlsr
        // lsr命令はレジスタオペランドがシフトされるmov命令と同じ
        print_data_process("mov", word);
        return 1;

    } else if (0xE2800000 == (word & 0xFFF00000)) {

        print_data_process("add", word);
        return 1;

    } else if (0xE3500000 == (word & 0xE3500000)) {

        print_data_process("cmp", word);
        return 1;

    } else if (0xE2000000 == (word & 0xFFF00000)) {

        print_data_process("and", word);
        return 1;

    } else if (0xE2400000 == (word & 0xFFF00000)) {

        print_data_process("sub", word);
        return 1;


    } else if (0xEA000000 == (word & 0xFF000000)) {

        print_branch_and_branch_with_link("b", word);
        return 1;

    } else if (0xEB000000 == (word & 0xFF000000)) {

        print_branch_and_branch_with_link("bl", word);
        return 1;

    } else if (0xAA000000 == (word & 0xFF000000)) {

        print_branch_and_branch_with_link("bge", word);
        return 1;

    } else if (0xBA000000 == (word & 0xFF000000)) {

        print_branch_and_branch_with_link("blt", word);
        return 1;

    } else if (0x1A000000 == (word & 0xFF000000)) {

        print_branch_and_branch_with_link("bne", word);
        return 1;

    } else if (0xE59F0000 == (word & 0xFFFF0000)) {

        print_data_transfer("ldr", word);
        return 1;

    } else if (0xE5D00000 == (word & 0xFFF00000)) {

        print_data_transfer("ldrb", word);
        return 1;

    } else if (0xE5800000 == (word & 0xFFF00000)) {

        print_data_transfer("str", word);
        return 1;


    } else if (0xE92D0000 == (word & 0xE92D0000)) {

        print_block_data_transfer("push", word);
        return 1;

    } else if (0xE8BD0000 == (word & 0xE8BD0000)) {

        print_block_data_transfer("pop", word);
        return 1;

    } else {

        // 実装なし
    }

    return 0;
}



void print_data_transfer(char* mnemonic, int word) {
    int immediate_value = word & 0xFFF;
    int source_or_destination_reg = (word >> 12) & 0xF;
    int base_reg = (word >> 16) & 0xF;

    if (immediate_value != 0) {
        printf("%s r%x, [r%d, #0x%x]", mnemonic, source_or_destination_reg, base_reg, immediate_value);

    } else {
        printf("%s r%x, [r%d]", mnemonic, source_or_destination_reg, base_reg);

    }
}


void print_branch_and_branch_with_link(char *mnemonic, int word) {

    int actual_offset;
    int is_twos_complement = (word >> 23) & 0b1;
    char *sign = "0x";

    int offset = word & 0xFFFFFF;

    if (is_twos_complement) {
        // 実際のオフセットは、2bit左にシフトした値
        // 二の補数は、bit反転させて0b1を足してマスクすることで得られる
        actual_offset = ((~offset + 0b1) & 0xFFFFFF) << 2;
        sign = "-0x";

    } else {
        actual_offset = offset << 2;
    }

    printf("%s [r15, #%s%x]",mnemonic, sign, actual_offset);
}


void print_block_data_transfer(char *mnemonic, int word){

    int register_list = word & 0xFFFF;

    struct register_array reg_arr = {.len=0};

    // スタックに渡すor取り出すレジスタを探し、配列に渡す
    for (int i = 0; i < 16; ++i) {
        int is_one = (register_list >> i) & 0b1;

        if (is_one) {
            reg_arr.list[reg_arr.len] = i;
            reg_arr.len++;
        }
    }

    char output_word[100];
    strcpy(output_word, "{");


    for (int j = 0; j < reg_arr.len; ++j) {
        char buf[10];
        //itoa(reg_arr.list[j], buf, 10);
        sprintf(buf, "%d", reg_arr.list[j]);

        strcat(output_word, "r");
        strcat(output_word, buf);

        if (j != (reg_arr.len - 1)) {
            strcat(output_word, ", ");
        }
    }
    strcat(output_word, "}");

    printf("%s %s",mnemonic, output_word);
}


void print_data_process(char *mnemonic, int word){

    int is_immediate = (word >> 25) & 0b1;
    int op_code = (word >> 21) & 0xF;
    int operand_1st_reg = (word >> 16) & 0xF;
    int destination_reg = (word >> 12) & 0xF;

    if (is_immediate) {
        int immediate_value = word & 0xFFF;
        int rotate_times = ((immediate_value >> 8) & 0xF) * 2;

        if (rotate_times != 0) {
            unsigned int lower_8bit = (unsigned int) immediate_value & 0xFF;

            // 4bitローテート
            immediate_value = (lower_8bit >> rotate_times)
                                | (lower_8bit << (32-rotate_times));
        }

        if (op_code == 0xD) {
            // mov
            printf("%s r%d, #0x%x", mnemonic, destination_reg, immediate_value);

        } else if (op_code == 0xA) {
            // cmp
            printf("%s r%d, #0x%x", mnemonic, operand_1st_reg, immediate_value);

        } else {
            // and, add, sub
            printf("%s r%d, r%d, #0x%x", mnemonic, destination_reg, operand_1st_reg, immediate_value);
        }

    } else {
        int operand_2nd_reg = word & 0xf;
        int shift_type = (word >> 5) & 0b11;
        int shift_reg =  (word >> 8) & 0xf;

        if (shift_type == 0x01) {
            printf("lsr r%d, r%d, r%d", destination_reg, operand_2nd_reg, shift_reg);
        } else {
            printf("%s r%d, r%d", mnemonic, destination_reg, operand_2nd_reg);
        }
    }
}


void print_hex_dump(int word){
    // 16進数ダンプ
    for (int i = 0; i < 4; ++i) {
        int two_digit_word = word & 0xFF;

        if (i != 3) {
            printf("%02x ", two_digit_word);
        } else {
            printf("%02x", two_digit_word);
        }
        word = word >> 2*4;
    }
}

int read_binary_file(struct Emitter *emitter) {

    int memory_address = 0x00010000;
    int is_instruction = 1;

    for (int i = 0; i < emitter->pos; ++i) {

        printf("0x%08x  ", memory_address);

        if (is_instruction) {
            is_instruction = print_asm(emitter->array[i]);
            if (is_instruction != 1) {
                print_hex_dump(emitter->array[i]);
            }
        } else {
            print_hex_dump(emitter->array[i]);
        }

        printf("\n");
        memory_address += 4;
    }

    return 0;
}
