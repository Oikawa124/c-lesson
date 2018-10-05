#include <assert.h>
#include "clesson.h"

struct KeyValue dict_array[1024];
static int dict_pos;


static int find_index(char *key){
    for (int index = 0; index < dict_pos; index++) {
        if (streq(key, dict_array[index].key)) {
            return index;
        }
    }
    return -1;
}

void dict_put(char *key, struct Element *elem){
    static int index;
    index = find_index(key);

    if (index != -1){
        dict_array[index].value = *elem;
    } else {
        dict_array[dict_pos].key = key;
        dict_array[dict_pos].value = *elem;
        dict_pos++;
    }
}


int dict_get(char *key, struct Element *out_elem){
    static int index;
    index = find_index(key);

    if (index != -1) {
        *out_elem = dict_array[index].value;
        return 1;
    }
    return 0;
}


void dict_print_all(){
    if (streq(dict_array[0].key, "no_key")){
        printf("NO DICT ELEMENT");
    }

    for (int i=0; i < dict_pos; i++){
        if (dict_array[i].value.etype == NUMBER) {
            printf("dict[%d]-> key:%s, Value: type:%d number %d\n", i, dict_array[i].key,
                   dict_array[i].value.etype, dict_array[i].value.u.number);
        } else {
            printf("dict[%d]-> key:%s, Value: type:%d name %s\n", i, dict_array[i].key,
                   dict_array[i].value.etype, dict_array[i].value.u.name);
        }
    }
}

void dict_clean(){
    dict_pos = 0;
}

void dict_init(){
    static int dict_pos = 0;
    struct KeyValue keyvalue = {"no_key",};
    dict_array[dict_pos] = keyvalue;
}

static void test_dict_one_times(){
    char *input_key = "plus";
    struct Element expect_elem = {ELEMENT_LITERAL_NAME, {"plus"}};

    dict_put(input_key, &expect_elem);

    struct Element actual = {NO_ELEMENT, {0}};
    dict_get(input_key, &actual);

    assert(streq(expect_elem.u.name, actual.u.name));

    dict_clean();
}

static void test_dict_two_times(){
    char *input_key1 = "plus";
    struct Element input_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    char *input_key2 = "mum";
    struct Element input_elem2 = {ELEMENT_NUMBER, {1}};

    dict_put(input_key1, &input_elem1);
    dict_put(input_key2, &input_elem2);


    assert(streq(input_key1, dict_array[0].key));
    assert(streq(input_elem1.u.name, dict_array[0].value.u.name));

    assert(streq(input_key2, dict_array[1].key));
    assert(input_elem2.u.number == dict_array[1].value.u.number);

    dict_print_all();
    dict_clean();
}

static void test_dict_same_key(){
    char *input_key = "plus";
    struct Element expect_elem = {ELEMENT_LITERAL_NAME, {"plus2"}};

    dict_put(input_key, &expect_elem);
    dict_put(input_key, &expect_elem);

    assert(streq(input_key, dict_array[0].key));
    assert(streq(expect_elem.u.name, dict_array[0].value.u.name));

    dict_clean();
}

static void test_dict_get(){
    struct Element expect_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    int expect_dict_get_result = 1;

    char *input_key = "plus";

    dict_put(input_key, &expect_elem1);

    struct Element actual = {NO_ELEMENT, {0}};

    int dict_get_result = dict_get(input_key, &actual);

    assert(expect_dict_get_result == dict_get_result);
    assert(expect_elem1.etype == actual.etype);
    assert(streq(expect_elem1.u.name, actual.u.name));

    dict_clean();
}

static void unit_test(){
    test_dict_one_times();
    test_dict_two_times();
    test_dict_same_key();
    test_dict_get();
}


//int main(){
//    dict_init();
//    unit_test();
//    dict_print_all();
//    return 1;
//}

