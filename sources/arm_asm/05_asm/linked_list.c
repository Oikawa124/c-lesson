#include <malloc.h>
#include <stdio.h>
#include <assert.h>

#include "asm.h"

B_list *b_list_head = NULL;


static B_list *create_b_list(int emit_arr_pos,
                             unsigned int op_address,
                             int label_symbol){
    B_list *b_list;

    b_list = malloc(sizeof(B_list));

    b_list->emit_arr_pos = emit_arr_pos;
    b_list->op_address = op_address;
    b_list->label_symbol = label_symbol;
    b_list->next = NULL;

    return b_list;
}

void add_b_list(int emit_arr_pos,
                unsigned int op_address,
                int label_symbol){

    if (b_list_head == NULL) {
        b_list_head = create_b_list(emit_arr_pos, op_address, label_symbol);
        return;
    }

    B_list *new_b_list = create_b_list(emit_arr_pos, op_address, label_symbol);

    B_list *pos = b_list_head;
    B_list *prev = NULL;

    // 最後の要素まで移動
    while (pos != NULL){
        prev = pos;
        pos = pos->next;
    }

    prev->next = new_b_list;
}


void initialize_linked_list(){

    B_list *tmp;
    while (b_list_head != NULL){
        tmp = b_list_head;
        b_list_head = b_list_head->next;
        free(tmp);
    }
    b_list_head = NULL;
}




/* unit tests*/
static void test_add_b_list_when_call_once(){

    // SetUP
    int input_emit_arr_pos = 1;
    unsigned int input_op_address = 0x000000004;
    int input_label_symbol = 1;

    // Exercise
    add_b_list(input_emit_arr_pos, input_op_address, input_label_symbol);

    // Verify
    assert(input_emit_arr_pos == b_list_head->emit_arr_pos);
    assert(input_op_address == b_list_head->op_address);
    assert(input_label_symbol == b_list_head->label_symbol);

    // TearDown
    initialize_linked_list();
}

static void test_add_b_list_when_call_two_times(){

    // SetUP
    int input_emit_arr_pos1 = 1;
    unsigned int input_op_address1 = 0x000000004;
    int input_label_symbol1 = 1;


    int input_emit_arr_pos2 = 5;
    unsigned int input_op_address2 = 0x0000FFF4;
    int input_label_symbol2 = 10;

    // Exercise
    add_b_list(input_emit_arr_pos1, input_op_address1, input_label_symbol1);
    add_b_list(input_emit_arr_pos2, input_op_address2, input_label_symbol2);

    // Verify
    assert(input_emit_arr_pos1 == b_list_head->emit_arr_pos);
    assert(input_op_address1 == b_list_head->op_address);
    assert(input_label_symbol1 == b_list_head->label_symbol);

    assert(input_emit_arr_pos2 == b_list_head->next->emit_arr_pos);
    assert(input_op_address2 == b_list_head->next->op_address);
    assert(input_label_symbol2 == b_list_head->next->label_symbol);

    // TearDown
    initialize_linked_list();
}


static void unit_tests(){

    test_add_b_list_when_call_once();
    test_add_b_list_when_call_two_times();
}

int main(){
    unit_tests();
    return 0;
}