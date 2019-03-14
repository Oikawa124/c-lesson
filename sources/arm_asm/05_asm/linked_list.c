#include <malloc.h>
#include <stdio.h>
#include <assert.h>

#include "asm.h"

unresolve_list *unresolve_list_head = NULL;


static unresolve_list *create_unresolve_list(int mnemonic,
                                             int emit_arr_pos,
                                             unsigned int op_address,
                                             int label_symbol){
    unresolve_list *unresole_list_;

    unresole_list_ = malloc(sizeof(unresolve_list));

    unresole_list_->mnemonic = mnemonic;
    unresole_list_->emit_arr_pos = emit_arr_pos;
    unresole_list_->op_address = op_address;
    unresole_list_->label_symbol = label_symbol;
    unresole_list_->next = NULL;

    return unresole_list_;
}

void add_unresolve_list(int mnemonic,
                        int emit_arr_pos,
                        unsigned int op_address,
                        int label_symbol){

    if (unresolve_list_head == NULL) {
        unresolve_list_head = create_unresolve_list(mnemonic, emit_arr_pos, op_address, label_symbol);
        return;
    }

    unresolve_list *new_unresolve_list = create_unresolve_list(mnemonic, emit_arr_pos, op_address, label_symbol);


    unresolve_list *node = unresolve_list_head;

    unresolve_list *prev_node = NULL;

    while (node != NULL) {
        prev_node = node;
        node = node->next;
    }

    prev_node->next = new_unresolve_list;

}


void free_linked_list(){

    unresolve_list *tmp, *temp_head;
    temp_head = unresolve_list_head;
    while (temp_head != NULL){
        tmp = temp_head;
        temp_head = temp_head->next;
        free(tmp);
    }
    unresolve_list_head = NULL;
}




/* unit tests*/
static void test_add_unresolve_list_when_call_once(){

    // SetUP
    int input_emit_arr_pos = 1;
    unsigned int input_op_address = 0x000000004;
    int input_label_symbol = 1;
    int input_mnemonic = B;

    // Exercise
    add_unresolve_list(input_mnemonic, input_emit_arr_pos, input_op_address, input_label_symbol);

    // Verify
    assert(input_mnemonic == unresolve_list_head->mnemonic);
    assert(input_emit_arr_pos == unresolve_list_head->emit_arr_pos);
    assert(input_op_address == unresolve_list_head->op_address);
    assert(input_label_symbol == unresolve_list_head->label_symbol);

    // TearDown
    free_linked_list();
}

static void test_add_unresolve_list_when_call_two_times(){

    // SetUP
    int input_emit_arr_pos1 = 1;
    unsigned int input_op_address1 = 0x000000004;
    int input_label_symbol1 = 1;
    int input_mnemonic = B;

    int input_emit_arr_pos2 = 5;
    unsigned int input_op_address2 = 0x0000FFF4;
    int input_label_symbol2 = 10;
    int input_mnemonic2 = B;

    // Exercise
    add_unresolve_list(input_mnemonic, input_emit_arr_pos1, input_op_address1, input_label_symbol1);
    add_unresolve_list(input_mnemonic2, input_emit_arr_pos2, input_op_address2, input_label_symbol2);

    // Verify
    assert(input_mnemonic == unresolve_list_head->mnemonic);
    assert(input_emit_arr_pos1 == unresolve_list_head->next->emit_arr_pos);
    assert(input_op_address1 == unresolve_list_head->next->op_address);
    assert(input_label_symbol1 == unresolve_list_head->next->label_symbol);

    assert(input_mnemonic2 == unresolve_list_head->mnemonic);
    assert(input_emit_arr_pos2 == unresolve_list_head->emit_arr_pos);
    assert(input_op_address2 == unresolve_list_head->op_address);
    assert(input_label_symbol2 == unresolve_list_head->label_symbol);

    // TearDown
    free_linked_list();
}


static void unit_tests(){

    test_add_unresolve_list_when_call_once();
    test_add_unresolve_list_when_call_two_times();
}


//int main(){
//    unit_tests();
//    return 0;
//}