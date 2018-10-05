#include <assert.h>
#include "clesson.h"

static int dict_pos = 0;
struct KeyValue {
    char *key;
    struct Element value;
};
static struct KeyValue dict_array[1024];


void dict_put(char *key, struct Element *elem){
    static int same_key_flag = 0;
    static int same_key_pos;

    if (dict_pos >= 1) {
        for (int i = 0; i < dict_pos; i++) {
            if (streq(key, dict_array[i].key)) {
                same_key_flag = 1;
                same_key_pos = i;
            }
        }
    }

    if (same_key_flag){
        dict_array[same_key_pos].value = *elem;
        same_key_flag = 0;
    } else {
        struct KeyValue key_value = {key, *elem};
        dict_array[dict_pos] = key_value;
        dict_pos++;
    }

}


int dict_get(char *key, struct Element *out_elem){
    for (int i=0; i < dict_pos; i++) {
        if (streq(key, dict_array[i].key)){
            *out_elem = dict_array[i].value;
            return 1;
        }
    }
    return 0;
}


void dict_print_all(){
    for (int i=0; i < dict_pos; i++){
        printf("dict[%d]-> key:%s, Value: type:%d value %s\n", i, dict_array[i].key,
                dict_array[i].value.etype, dict_array[i].value.u.name);
    }
}

void dict_clean(){
    dict_pos = 0;
}

static void test_dict_one_times(){
    char *expect_key = "plus";
    struct Element expect_elem = {ELEMENT_LITERAL_NAME, {"plus"}};

    char *actual_key = "plus";
    struct Element actual_elem = {ELEMENT_LITERAL_NAME, {"plus"}};

    dict_put(actual_key, &actual_elem);

    assert(expect_key == dict_array[0].key);
    assert(expect_elem.u.name == dict_array[0].value.u.name);

    dict_clean();
}

static void test_dict_two_times(){
    char *expect_key1 = "plus";
    struct Element expect_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    char *expect_key2 = "minus";
    struct Element expect_elem2 = {ELEMENT_LITERAL_NAME, {"minus"}};

    char *actual_key1 = "plus";
    struct Element actual_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    char *actual_key2 = "minus";
    struct Element actual_elem2 = {ELEMENT_LITERAL_NAME, {"minus"}};

    dict_put(actual_key1, &actual_elem1);
    dict_put(actual_key2, &actual_elem2);

    assert(expect_key1 == dict_array[0].key);
    assert(expect_elem1.u.name == dict_array[0].value.u.name);

    assert(expect_key2 == dict_array[1].key);
    assert(expect_elem2.u.name == dict_array[1].value.u.name);

    dict_clean();
}

static void test_dict_same_key(){
    char *expect_key = "plus";
    struct Element expect_elem = {ELEMENT_LITERAL_NAME, {"plus2"}};

    char *actual_key1 = "plus";
    struct Element actual_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    char *actual_key2 = "plus";
    struct Element actual_elem2 = {ELEMENT_LITERAL_NAME, {"plus2"}};

    dict_put(actual_key1, &actual_elem1);
    dict_put(actual_key2, &actual_elem2);

    assert(expect_key == dict_array[0].key);
    assert(expect_elem.u.name == dict_array[0].value.u.name);

    dict_clean();
}

static void test_dict_get(){
    struct Element expect_elem1 = {ELEMENT_LITERAL_NAME, {"plus"}};
    int expect_dict_get_result = 1;

    char *key = "plus";
    struct Element elem = {ELEMENT_LITERAL_NAME, {"plus"}};

    dict_put(key, &elem);

    struct Element actual = {NO_ELEMENT, {0}};

    int dict_get_result = dict_get(key, &actual);

    assert(expect_dict_get_result == dict_get_result);
    assert(expect_elem1.etype == actual.etype);
    assert(expect_elem1.u.name == actual.u.name);

    dict_clean();
}

static void unit_test(){
    test_dict_one_times();
    test_dict_two_times();
    test_dict_same_key();
    test_dict_get();
}


int main(){
    unit_test();
//    dict_print_all();
    return 1;
}

