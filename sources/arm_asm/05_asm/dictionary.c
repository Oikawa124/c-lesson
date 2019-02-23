#include <stdio.h>
#include <assert.h>

#define MAX_SIZE 1024
#define INDEX_NOT_EXIST -1

static int dict_pos = 0;

struct KeyValue{
    int key;
    unsigned int value;
};

static struct KeyValue dict_array[MAX_SIZE];


static int find_index(int key){
    for (int index = 0; index < dict_pos; ++index) {
        if (key == dict_array[index].key) {
            return index;
        }
    }
    return INDEX_NOT_EXIST;
}

int dict_get(int key, unsigned int *out_line_num){

    int index = find_index(key);

    if (index != INDEX_NOT_EXIST) {
        *out_line_num = dict_array[index].value;
        return 1;
    }
    return 0;
}

void dict_put(int key, unsigned int line_num){

    int index = find_index(key);

    if (index != INDEX_NOT_EXIST) {
        dict_array[index].value = line_num;
    } else {
        dict_array[dict_pos].key = key;
        dict_array[dict_pos].value = line_num;
        dict_pos++;
    }
}

void dict_print_all(){
    if (dict_pos == 0) {
        printf("Not Exist");
    }

    for (int index = 0; index < dict_pos; ++index) {
        printf("Key: %d, Value: %d",
               dict_array[index].key, dict_array[index].value);
    }
}


void initialize_dict(){
    for (int index = 0; index < dict_pos; ++index) {
        dict_array->key = 0;
        dict_array->value = 0;
    }
    dict_pos = 0;
}



/*unit tests*/
static void test_dict_put_when_call_once(){

    // SetUP
    int expect_key = 1;
    unsigned int expect_value = 0x00010004;

    // Exercise
    dict_put(expect_key, expect_value);

    int actual_key = dict_array[0].key;
    unsigned int actual_value = dict_array[0].value;

    // Verify
    assert(expect_key == actual_key);
    assert(expect_value == actual_value);

    // TearDown
    initialize_dict();
}

static void test_dict_put_when_call_three_times(){

    // SetUP
    int expect_key1 = 1;
    int expect_key2 = 2;
    int expect_key3 = 3;

    unsigned int expect_value1 = 0x00010004;
    unsigned int expect_value2 = 0x000100a6;
    unsigned int expect_value3 = 0x0001FFFF;

    // Exercise
    dict_put(expect_key1, expect_value1);
    dict_put(expect_key2, expect_value2);
    dict_put(expect_key3, expect_value3);

    int actual_key1   = dict_array[0].key;
    unsigned int actual_value1 = dict_array[0].value;

    int actual_key2   = dict_array[1].key;
    unsigned int actual_value2 = dict_array[1].value;

    int actual_key3   = dict_array[2].key;
    unsigned int actual_value3 = dict_array[2].value;

    // Verify
    assert(expect_key1 == actual_key1);
    assert(expect_value1 == actual_value1);

    assert(expect_key2 == actual_key2);
    assert(expect_value2 == actual_value2);

    assert(expect_key3 == actual_key3);
    assert(expect_value3 == actual_value3);

    // TearDown
    initialize_dict();
}

static void test_dict_put_when_same_key(){

    // SetUP
    int input_key1 = 1;
    unsigned int input_value1 = 0x00010004;

    int input_key2 = 1;
    unsigned int input_value2 = 0x00011d11;

    int expect_value = 0x00011d11;

    // Exercise
    dict_put(input_key1, input_value1);
    dict_put(input_key2, input_value2);

    unsigned int actual_value;
    dict_get(input_key2, &actual_value);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_dict();
}

static void test_dict_get_when_call_once(){

    // SetUP
    int input_key = 1;
    unsigned int expect_value = 0x00010004;

    // Exercise
    dict_put(input_key, expect_value);

    unsigned int actual_value;
    dict_get(input_key, &actual_value);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_dict();
}

static void test_dict_get_when_call_three_times(){

    // SetUP
    int input_key1 = 1;
    int input_key2 = 2;
    int input_key3 = 3;

    unsigned int expect_value1 = 0x00010004;
    unsigned int expect_value2 = 0x00010a01;
    unsigned int expect_value3 = 0x00010e04;

    // Exercise
    dict_put(input_key1, expect_value1);
    dict_put(input_key2, expect_value2);
    dict_put(input_key3, expect_value3);


    unsigned int actual_value1;
    dict_get(input_key1, &actual_value1);

    unsigned int actual_value2;
    dict_get(input_key2, &actual_value2);

    unsigned int actual_value3;
    dict_get(input_key3, &actual_value3);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);
    assert(expect_value3 == actual_value3);

    // TearDown
    initialize_dict();
}

void unit_tests() {

    /*dict put*/
    test_dict_put_when_call_once();
    test_dict_put_when_call_three_times();
    test_dict_put_when_same_key();

    /*dict get*/
    test_dict_get_when_call_once();
    test_dict_get_when_call_three_times();
}

//int main(){
//
//    unit_tests();
//
//    return 0;
//}